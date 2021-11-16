<?php

/** test the concepts, with php 8.0+ , especially the regular expressions implementation */
class TomlParser
{

    const tom_EOS = 0;
    const tom_Equal = 1;
    const tom_Bool = 2;
    const tom_DateTime = 3;
    const tom_FloatExp = 4;
    const tom_FloatDot = 5;
    const tom_Float_E = 6;
    const tom_Integer = 7; 
    const tom_Quote3 = 8;
    const tom_Quote1 = 9;
    const tom_Apost3 = 10;
    const tom_Apost1 = 11;
    const tom_NewLine = 12; 
    const tom_Space = 13;
    const tom_LSquare = 14; 
    const tom_RSquare = 15;
    const tom_LCurly = 16;
    const tom_RCurly = 17;
    const tom_Comma = 18;
    const tom_Dot = 19;
    const tom_BareKey = 20; 
    const tom_EscapedChar = 21;
    const tom_Escape = 22;
    const tom_NoEscape = 23;
    const tom_Hash = 24;
    const tom_LitString = 25;
    const tom_SpacedEqual = 26;
    const tom_Comment = 27;
    const tom_HashComment = 28;
    const tom_AnyValue = 29;
    const tom_Dig_Dig = 30;
    const tom_No_0Digit = 31;
    const tom_AnyChar = 32;

    const PARTIAL = 1;
    const MATCH = 2;
    const FAIL = 4;
    
    protected array $root;   // root array
    protected array $table_parts = []; // store meta data on paths
    protected array $table;  // reference original
    protected int $index;
    protected string $hold;
    protected int $slen;
    protected array $map;
    protected bool $flagLF;
    protected array $expSet;
    protected int $tokenLine;
    protected array $key_exp = [self::tom_Space, self::tom_BareKey, self::tom_Integer];
    protected array $value_exp = [self::tom_Bool, self::tom_DateTime, self::tom_FloatExp, self::tom_FloatDot];
    protected array $estr_exp = [self::tom_Space, self::tom_NoEscape, self::tom_EscapedChar, self::tom_Quote3];
    protected array $lstr_exp = [self::tom_LitString];
    protected array $ml_exp = [self::tom_LitString, self::tom_Apost3];
    
    protected string $int_exp;
    protected string $float_exp;
    protected string $float_dot;

    // current token
    public string $value;
    public int $code; // values at the start of the match;
    public int $id; // match regular expression token id
    public int $line; // line number in stream;
    public bool $isSingle; // if single character match
    public int $lineBegin;
    
    static function initMap(): array
    {
        $exp = [
            self::tom_Bool => '(true|false)',
            self::tom_DateTime => '(\d{4}-\d{2}-\d{2}(T\d{2}:\d{2}:\d{2}(\.\d{6})?(Z|-\d{2}:\d{2})?)?)',
            self::tom_FloatExp => '([+-]?((\d_?)+([\.](\d_?)*)?)([eE][+-]?(_?\d_?)+))',
            self::tom_FloatDot => '([+-]?((\d_?)+([\.](\d_?)*)))',
            self::tom_Integer => '([+-]?(\d_?)+)',
            self::tom_Quote3 => '(\"\"\")',
            self::tom_Apost3 => '(\'\'\')',
            self::tom_Space => '(\h+)',
            self::tom_BareKey => '([-A-Z_a-z0-9]+)',
            self::tom_EscapedChar => '(\\\(n|t|r|f|b|\\"|\\\\|u[0-9A-Fa-f]{4,4}|U[0-9A-Fa-f]{8,8}))',
            self::tom_NoEscape => '([^\x{0}-\x{19}\x{22}\x{5C}]+)',
            self::tom_LitString => '([^\x{0}-\x{19}\x{27}]+)',
            self::tom_AnyValue => '([^\s\]\},]+)',
            self::tom_SpacedEqual => '(\h*=\h*)',
            self::tom_Comment => '(\V*)',
            self::tom_HashComment => '(\h*#\V*|\h*)',
            self::tom_Dig_Dig => '([^\d]_[^\d])|(_\$)',
            //self::tom_No_0Digit = "^(0\d+)",
            self::tom_Float_E => '([^\d]_[^\d])|_[eE]|[eE]_|(_\$)',
        ];

        $map = [];
        foreach ($exp as $ix => $val) {
            $map[$ix] = "/\\G" . $val . "/u";
        }
        return $map;
    }

    public function firstMatch(): array
    {
        foreach ($this->expSet as $id) {
            $exp = $this->map[$id];
            $match = null;
            $res = preg_match($exp, $this->hold, $match, 0, $this->index);
            if (!empty($match)) {
                return [$id, $match];
            }
        }
        return [0, null];
    }

    /**
     * Update token with next character and code in subject 
     * @return bytes length of character
     */
    public function peekChar(): int
    {
        if ($this->index < $this->slen) {
            $code = 0;
            $value = str_utf8c($this->hold, $this->index, $code);
            if ($code !== -1) {
                $this->code = $code;
                $this->value = $value;
                return strlen($value);
            }
            $this->id = self::tom_EOS;
            $this->code = -1;
            $this->value = '';
            $this->isSingle = true;
        }
        return 0;
    }

    /**
     * 
     * @return type
     * @throws ExceptionCheck that a control-code ascii < 20
     * is a CR or LF. Skip CR.
     */
    public function checkLF()
    {
        if ($this->code === 13) {
            $this->index += 1;
            $this->peekChar();
        }
        if ($this->code === 10) {
            $this->flagLF = true;
            $this->id = self::tom_NewLine;
            $this->line = $this->tokenLine;
            $this->isSingle = true;
            $this->lineBegin = $this->index + 1;
            return;
        }
        throw new Exception("Invalid character code: " . dechex($this->code));
    }

    public function peekToken(): int
    {
        $csize = $this->peekChar();
        if ($csize === 0) {
            $this->line = $this->tokenLine;
        } else if ($this->code < 20) {
            $this->checkLF();
        } else {
            $this->line = $this->tokenLine;
            $this->isSingle = false;
            if ($csize === 1) {
                $fit = $this->singleMatch($this->code);
                if ($fit) {
                    $this->id = $fit;
                    $this->isSingle = true;
                } else {
                    $this->id = self::tom_AnyChar;
                }
            } else {
                $this->id = self::tom_AnyChar;
            }
        }
        return $this->id;
    }

    public function singleMatch(int $code): int
    {
        switch ($code) {
            case 45: return self::tom_Equal;
            case 91: return self::tom_LSquare;
            case 93: return self::tom_RSquare;
            case 46: return self::tom_Dot;
            case 44: return self::tom_Comma;
            case 123: return self::tom_LCurly;
            case 125: return self::tom_RCurly;
            case 39: return self::tom_Apost1;
            case 34: return self::tom_Quote1;
            case 35: return self::tom_Hash;
            case 92: return self::tom_Escape;
            case 32: return self::tom_Space;
            case 9: return self::tom_Space;
            case 12: return self::tom_Space;
            case 8: return self::tom_Space;
            default: return self::tom_AnyValue;
        }
    }

    public function moveNext(): int
    {

        if ($this->flagLF) {
            $this->flagLF = false;
            $this->tokenLine += 1;
            $this->line = $this->tokenLine;
        }
        $nextCt = $this->peekChar();
        if ($nextCt === 0) {
            $this->id = self::tom_EOS;
        } else if ($this->code < 20) {
            $this->checkLF();
            $this->index++;
        } else {
            [$id, $capture] = $this->firstMatch();
            if ($id > 0) {
                $this->id = $id;
                $this->value = $capture[1];
                $this->isSingle = false;
                $this->index += strlen($capture[1]);
            } else {
                if ($id > 0) {
                    throw new Exception('Bad capture array');
                }
                $this->index += $nextCt;
                $this->isSingle = false;
                if ($nextCt === 1) {
                    $fit = $this->singleMatch($this->code);
                    if ($fit) {
                        $this->id = $fit;
                        $this->isSingle = true;
                    } else {
                        $this->id = self::tom_EOS;
                    }
                } else {
                    $this->id = self::tom_EOS;
                }
            }
        }
        $this->line = $this->tokenLine;
        return $this->id;
    }

    function moveExpr(int $id): bool
    {
        $expr = $this->map[$id];
        if ($this->index < $this->slen) {
            // rematch for a chosen expression
            $match = null;
            $res = preg_match($this->map[$id], $this->hold, $match, 0, $this->index);
            if (is_integer($res) && count($match) > 1) {

                $this->value = $match[1];
                $next = strlen($match[0]);
                $this->index += $next;
                $this->id = self::tom_AnyChar;
                $this->isSingle = false;
                return true;
            }
        }
        return false;
    }

    function comment(): bool
    {
        return $this->moveExpr(self::tom_Comment);
    }

    function integer(string $s): string
    {
        if ((substr($s, -1) === "_") || (substr($s, 0, 1) === "_")) {
            $this->valueError("Invalid integer: Underscore must be between digits", $s);
        }

        return str_replace('_', '', $s);
    }


    function escape_string(string $s): string
    {
        $val = substr($s, 1, 1);
        switch ($val) {
            case 'n':
                return "\n";
            case 't':
                return "\t";
            case 'r':
                return "\r";
            case 'b':
                return "\b";
            case 'f':
                return "\f";
            case '"':
                return "\"";
            case '\\':
                return "\\";
            case 'u':
                return IntlChar::chr(hexdec(substr($s, 2, 4)));
            case 'U':
                return IntlChar::chr(hexdec(substr($s, 2, 8)));
            default:
                $this->svalueError("Invalid ecape character ", $val);
                break;
        }
        return '';
    }

    function doneToken()
    {
        if ($this->flagLF) {
            $this->flagLF = false;
            $this->tokenLine += 1;
        }
        $this->line = $this->tokenLine;
        $id = $this->id;
        if ($id === self::tom_EOS) {
            return;
        }
        if ($id === self::tom_NewLine) {
            $this->index++;
            return;
        }
        $this->index += strlen($this->value);
    }

    function literalStr(): string
    {
        $ret = '';

        $id = $this->peekToken();
        while ($id !== self::tom_Apost1) {
            if ($id === self::tom_NewLine || $id === self::tom_EOS) {
                $this->syntaxError("Value without end quote (')");
            }
            if ($this->moveExpr(self::tom_LitString)) {
                $ret .= $this->value;
            } else {
                $this->syntaxError("Invalid string value");
            }
            $id = $this->peekToken();
        }
        $this->doneToken();
    }

    function syntaxError(string $msg)
    {
        throw new Exception("TOML error: " . $msg);
    }

    function valueError(string $msg, string $value)
    {
        throw new Exception($msg . " Value { $value }");
    }

    function keyName(): string
    {

        switch ($this->id) {
            case self::tom_BareKey:
                return $this->value;
            case self::tom_Quote1:
                return $this->quoteStr();
            case self::tom_Apost1:
                return $this->literalStr(name);
            case self::tom_Integer:
                return $this->integer($this->value);
            default:
                $this->syntaxError("Improper Key");
                break;
        };
        return '';
    }

    function listSame(): array
    {
        $ret = [];
        $inloop = true;
        $val0 = null;
        while ($inloop) {
            $id = $this->peekToken();
            switch ($id) {
                case self::tom_Space:
                    $this->moveExpr(self::tom_Space);
                    break;
                case self::tom_NewLine:
                    $this->doneToken();
                    break;
                case self::tom_Hash:
                    $this->doneToken();
                    $this->comment();
                    break;
                case self::tom_EOS:
                    $this->doneToken();
                    $this->syntaxError("Missing ']'");
                    break;
                case self::tom_RSquare:
                    $this->doneToken();
                default:
                    $inloop = false;
                    break;
            }
        }

        while ($id !== self::tom_RSquare) {
            if ($id == self::tom_LSquare) {
                $this->doneToken();
                if (count($ret) === 0) {
                    $val0 = 'array';
                } else {
                    if ($val0 !== 'array') {
                        $this->arrayTypeError($val0, 'array');
                    }
                }
                $ret[] = $this->listSame();
            } else {
                $value = $this->value();
                if (count($ret) === 0) {
                    $val0 = gettype($value);
                } else {
                    $valn = gettype($value);
                    if ($valn != $val0) {
                        $this->arrayTypeError($valn, $valn);
                    }
                }
                $ret[] = $value;
            }

            $inloop = true;
            $comma = false;
            while ($inloop) {
                $id = $this->peekToken();
                switch ($id) {
                    case self::tom_Space:
                        $this->moveExpr(self::tom_Space);
                        break;
                    case self::tom_NewLine:
                        $this->doneToken();
                        break;
                    case self::tom_Hash:
                        $this->doneToken();
                        $this->comment();
                        break;
                    case self::tom_Comma:
                        if ($comma) {
                            $this->syntaxError('No value between commas');
                        }
                        $comma = true;
                        $this->doneToken();
                        break;
                    case self::tom_EOS:
                        $this->doneToken();
                        $this->syntaxError("Missing ']'");
                        break;
                    case self::tom_RSquare:
                        $this->doneToken();
                    default:
                        $inloop = false;
                        break;
                }
            }
        }
        return $ret;
    }

    function inlineTable(): void
    {
        $id = $this->moveNext();
        if ($id === self::tom_Space) {
            $id = $this->moveNext();
        }

        if ($id !== self::tom__RCurly) {
            $this->keyValue();
            $id = $this->moveNext();
            if ($id === self::tom_Space) {
                $id = $this->moveNext();
            }
        }
        while ($id === self::tom_Comma) {
            $id = $this->moveNext();
            if ($id === self::tom_Space) {
                $id = $this->moveNext();
            }
            $this->keyValue();
            $id = $this->moveNext();
            if ($id === self::tom_Space) {
                $id = $this->moveNext();
            }
        }

        if ($id !== self::tom_RCurly) {
            $this->syntaxError("Inline table expects '}'");
        }
    }

    function arrayMatchError(string $a, string $b)
    {
        throw new Exception("Lists must be of same type: $a <> $b");
    }

    function keyValue()
    {
        $key = $this->keyName();
        if (isset($this->table[$key])) {
            $this->syntaxError("Duplicate key: $key");
        }
        if (!$this->moveExpr(self::tom_SpacedEqual)) {
            $this->syntaxError("Expect <key> = ");
        }

        $id = $this->peekToken();
        $rhs = null;

        if ($id === self::tom_LSquare) {
            $this->doneToken();
            $rhs = $this->listSame();
        } else if ($id === self::tom_LCurly) {
            $this->doneToken();
            $oldTable = &$this->table;
            $rhs = [];
            $this->table = &$rhs;
            $this->inlineTable();
            $this->table = &$oldTable;
        } else {
            $rhs = $this->value();
        }
        $this->table[$key] = $rhs;
    }

    function currentLine(): string
    {
        if ($this->lineBegin < $this->index) {
            return substr($this->hold,
                    $this->lineBegin,
                    $this->index - $this->lineBegin
            );
        }
    }

    function finishLine(): int
    {
        $this->moveExpr(self::tom_HashComment);
        $ret = $this->moveNext();
        if ($ret !== self::tom_NewLine && $ret !== self::tom_EOS) {
            $this->syntaxError("Non-comment at end of line");
        }
        return $ret;
    }

    function __construct() {
        $this->map = self::initMap();
        $this->int_exp = $this->map[self::tom_Integer];
        $this->float_exp = $this->map[self::tom_FloatExp];
        $this->float_dot = $this->map[self::tom_FloatDot];
    }
    function parse(string $src): array
    {
        $this->lineBegin = 0;
        $this->hold = $src;
        $this->index = 0;
        $this->slen = strlen($src);
        $this->flagLF = false;
        $this->tokenLine = 0;

        $this->root = [];
        $this->table = &$this->root;
        $this->expSet = $this->key_exp;

        try {
            $id = $this->moveNext();
            while ($id !== self::tom_EOS) {
                switch ($id) {
                    case self::tom_Hash:
                        $this->comment();
                        $id = $this->moveNext();
                        break;
                    case self::tom_Space:
                    case self::tom_NewLine:
                        $id = $this->moveNext();
                        break;
                    case self::tom_Quote1:
                    case self::tom_BareKey:
                    case self::tom_Apost1:
                    case self::tom_Integer:
                        $this->keyValue();
                        $id = $this->finishLine();
                        break;
                    case self::tom_LSquare:
                        $this->tablePath();
                        $id = $this->finishLine();
                        break;
                    default:
                        $this->syntaxError("Expect <key> =, [Path] or # Comment");
                        break;
                }
            }
        } catch (Exception $ex) {
            $msg = $ex->getMessage();
            $line = $this->tokenLine + 1;
            throw new Exception(
                            $msg . PHP_EOL .
                            "Error in source file line - $line :"
                            . PHP_EOL . $this->currentLine()
                            . PHP_EOL);
        }
        return $this->root;
    }

    function tablePath()
    {
        $isAOT = false;
        $hitNew = false;

        $path = '';

        $dotCount = 0;
        $aotLength = 0;
        $path_ref = &$this->root;

        $id = $this->moveNext();
        $inloop = true;
        $parts_ct = 0;
        
        while ($inloop) {
            switch ($id) {
                case self::tom_Hash:
                    $this->syntaxError("Unexpected '#' in path");
                    break;
                case self::tom_Equal:
                    $this->syntaxError("Unexpected '=' in path");
                    break;
                case self::tom_Space:
                    $id = $this->moveNext();
                    break;
                case self::tom_EOS:
                    $inloop = false;
                    break;
                case self::tom_NewLine:
                    $this->syntaxError("New line in unfinished path");
                    break;
                case self::tom_RSquare:
                    if ($isAOT) {
                        if ($aotLength == 0) {
                            $this->syntaxError("AOT Segment cannot be empty");
                        }
                        $isAOT = false;
                        $aotLength = 0;
                        $id = $this->moveNext();
                    } else {
                        $inloop = false;
                    }
                    break;
                case self::tom_LSquare:
                    if ($dotCount < 1 && strlen($path) > 0) {
                        $this->syntaxError("Expected a '.' after path part");
                    }
                    if ($isAOT) {
                        $this->syntaxError("Too many consecutive [ in path");
                    }
                    $id = $this->moveNext();
                    $isAOT = true;
                    break;
                case self::tom_Dot:
                    if ($dotCount > 0) {
                        $this->syntaxError("Too many consecutive . . in path");
                    }
                    $dotCount++;
                    $id = $this->moveNext();
                    break;
                case self::tom_Quote1:
                default:


                    $part_key = $this->keyName();
                    $path = $path . "[" . $part_key . "]";
                    
                    if (($dotCount < 1) && ($parts_ct > 0)) {
                        $this->syntaxError("Expected a '.' after path part");
                    }
                    $dotCount = 0;
                    $parts_ct += 1;
                    if (array_key_exists($part_key, $path_ref)) {
                        $path_ref = &$path_ref[$part_key];

                        // There's a key-value stored already

                        if (!is_array($path_ref)) {
                            // its a value and we want to set an object
                            $this->syntaxError("Duplicate key path: " .
                                    $path);
                        }
                        $tag = $this->table_parts[$path];
                        if ($tag->aot) {
                            $aotLength++;
                            //$lastAOT = $tag;
                            if (!$tag->implicit) {
                                $path_ref = &$tag->endRef();
                            }
                        }
                    } else {
                        // make new branch
                        $hitNew = true;

                        // first branch declaration has to be correct
                        if ($isAOT) {
                            $aotLength++;
                        }

                        $path_ref[$part_key] = [];
                        $path_ref = &$path_ref[$part_key];
                        $tag = new TomPartTag();

                        $tag->aot = $isAOT;
                        $tag->implicit = true;
                        $tag->n = 0;
                        $tag->base = &$path_ref;

                        $this->table_parts[$path] = $tag;
                    }

                    $id = $this->moveNext();
                    break;
            }// end switch
        } // end loop

        if (empty($path)) {
            $this->syntaxError("Table path cannot be empty");
        }
        if (!$hitNew) {
            // check last object && last parts value
            $tag = $this->table_parts[$path];

            if ($tag->aot) {
                $path_ref = &$tag->newRef();
            } else {
                // Table part created earlier, allow if it was just implicit creation
                if ($tag->implicit) {
                    $tag->implicit = false; // won't allow this to happen again
                } else {
                    $this->syntaxError("Duplicate key path: " . $path);
                }
            }
        } else {
            $tag = $this->table_parts[$path];
            if ($tag->aot) {
                $path_ref = &$tag->newRef();
            }
            $tag->implicit = false; // end of path cannot be implicit
        }
        $this->table = &$path_ref;
    }

    function litString(): string
    {
        $id = $this->peekToken();
        while ($id != self::tom_Apost1) {
            if ($id == self::tom_NewLine || $id == self::tom_EOS) {
                $this->syntaxError("Missing end quote ( ' )");
            }
            if ($this->moveExpr(self::tom_LitString)) {
                $ret .= $this->value;
            } else {
                $this->syntaxError("Bad literal string value");
            }
            $id = $this->peekToken();
        }
        $this->doneToken();
    }

    function matchInt(string $s, &$mixed): int {
        $match = null;
        $ct = preg_match($this->int_exp, $s, $match);
        if ($ct > 0) {
            $capture = $match[1];
            if (strlen($s) === strlen($capture)) {
                $mixed = intval($this->integer($capture));
                return self::MATCH;
            }
            return self::PARTIAL;
        }
        return self::FAIL;
    }
    
    function matchFloatDot(string $s, &$mixed): int 
    {
        $match = null;
        $ct = preg_match($this->float_dot, $s, $match);
        if ($ct > 0) {
            $capture = $match[1];
            if (strlen($s) === strlen($capture)) {
                $mixed = floatval($this->float($capture, $match));
                return self::MATCH;
            }
           return self::PARTIAL;
        }
        return self::FAIL;
    }
    function matchFloat(string $s, &$mixed): int 
    {
        $match = null;
        $ct = preg_match($this->float_exp, $s, $match);
        if ($ct > 0) {
            $capture = $match[1];
            if (strlen($s) === strlen($capture)) {
                $mixed =  floatval($this->floatExp($capture));
                return self::MATCH;
            }
            return self::PARTIAL;
        }
        return self::FAIL;
    }
    
    function matchBool(string $s, &$mixed) : int
    {
        $expr = $this->map[self::tom_Bool];
        $match = null;
        $ct = preg_match($expr, $s, $match);
        if ($ct > 0) {
            $capture = $match[1];
            if (strlen($s) === strlen($capture)) {
                $mixed = ($capture === "true") ? true : false;
                return self::MATCH;
            }
            return self::PARTIAL;
        }
        return self::FAIL;
    }
    
    function matchDateTime(string $s, &$mixed) : int 
    {
        $expr = $this->map[self::tom_DateTime];
        $match = null;
        $ct = preg_match($expr, $s, $match, 0, 0);
        if ($ct > 0) {
            $capture = $match[1];
             if (strlen($s) === strlen($capture)) {
                $mixed = new DateTime($capture);
                return self::MATCH;
            }
            return self::PARTIAL;
        }
        return self::FAIL;
    }
    
    function value()
    {
        $id = $this->id;
        if ($id === self::tom_Apost1) {
            if ($this->moveExpr(self::tom_Apost3)) {
                $value = $this->mlString();
            } else {
                $this->doneToken();
                $value = $this->literalStr();
            }
            return $value;
        } else if ($id === self::tom_Quote1) {
            if ($this->moveExpr(self::tom_Quote3)) {
                $value = $this->mlqString();
            } else {
                $this->doneToken();
                $value = $this->quoteStr();
            }
            return $value;
        }
        $notfull = false;
        if (!$this->moveExpr(self::tom_AnyValue)) {
            $this->syntaxError("No value after = ");
        } else {
            $value = $this->value;
            $full_value = $value;
            $result = null;
            
            $match = $this->matchInt($value,$result);
            if ($match === self::MATCH) {
                return $result;
            }
            else if (!$notfull) {
                $notfull = ($match===self::PARTIAL);
            }
            
            $match = $this->matchFloat($value,$result);
            if ($match === self::MATCH) {
                return $result;
            }
            else if (!$notfull) {
                $notfull = ($match===self::PARTIAL);
            }
            
            $match = $this->matchFloatDot($value,$result);
            if ($match === self::MATCH) {
                return $result;
            }
            else if (!$notfull) {
                $notfull = ($match===self::PARTIAL);
            }
            
            $match = $this->matchBool($value,$result);
            if ($match === self::MATCH) {
                return $result;
            }
            else if (!$notfull) {
                $notfull = ($match===self::PARTIAL);
            }
            
            $match = $this->matchDateTime($value, $result);
            if ($match === self::MATCH) {
                return $result;
            }
            else if (!$notfull) {
                $notfull = ($match===self::PARTIAL);
            }

        }
        if ($notfull) {
            throw new Exception("Cannot fully match: " . $full_value);
        }
        $this->syntaxError("No value type match found for " . $value);
    }

    function floatExp(string $s): string
    {
        $match = null;
        $ct = preg_match($this->map[self::tom_Float_E], $s, $match);
        if (ct > 0) {
            $this->valueError("Invalid float: Underscore must be between digits", $s);
        }
        $s = str_replace('_', '', $s);
        $ct = preg_match($this->map[self::tom_No_0Digit], $s, $match);
        if (ct > 0) {
            $this->valueError("Invalid float with exponent: Underscore must be between digits", $s);
        }
        return $s;
    }

    function mlqString(): string
    {
        $oldExpSet = $this->expSet;
        $this->expSet = $this->ml_exp;

        $id = $this->moveNext();

        if ($id === self::tom_NewLine) {
            $id = $this->moveNext();
        }

        $inloop = true;
        $ret = '';

        while ($inloop) {
            switch ($id) {
                case self::tom_Quote3:
                    $inloop = false;
                    break;
                case self::tom_EOS:
                    $this->syntaxError("Missing close quotes");
                    break;
                case self::tom_Escape:
                    do {
                        $id = $this->moveNext();
                    } while ($id === self::tom_Space || $id === self::tom_NewLine || $id == self::tomEscape);
                    break;
                case self::tom_Space:
                    $ret .= ' ';
                    $id = $this->moveNext();
                    break;
                case self::tom_NewLine:
                    $ret .= '\n';
                    $id = $this->moveNext();
                    break;
                case self::tom_EscapedChar:
                    $ret .= $this->escape_str($this->value);
                    $id = $this->moveNext();
                    break;
                default:
                    $ret .= $this->value;
                    $id = $this->moveNext();
                    break;
            }
        }
        $this->expSet = $oldExpSet;
        return $ret;
    }

    function quoteStr(): string
    {
        $oldExpSet = $this->expSet;
        $this->expSet = $this->estr_exp;
        $ret = '';
        $id = $this->moveNext();

        while ($id != self::tom_Quote1) {
            if ($id === self::tom_NewLine || $id === self::tom_EOS || $id === self::tom_Escape) {
                $this->syntaxError("Missing string end quote");
            } else if ($id === self::tom_EscapedChar) {
                $ret .= $this->escape_string($this->value);
            } else {
                $ret .= $this->value;
            }
            $id = $this->moveNext();
        }
        $this->expSet = $oldExpSet;
        return $ret;
    }

    function float(string $s, array $match): string
    {

        if (count($match) < 5) {
            $this->valueError("Wierd Float Capture", $s);
        }

        if (count($match[4]) <= 1) {
            $this->valueError("Float needs at least one digit after decimal point", $s);
        }
        $m2 = null;
        $ct = preg_match($this->map[self::tom_Dig_Dig], $s, $m2);

        if ($ct > 0) {
            $this->valueError("Invalid float: Underscore must be between digits", $s);
        }
        $s = str_replace('_', '', $s);

        $m2 = null;
        $ct = preg_match($this->map[self::tom_No_0Digit], $s, $m2);
        if ($ct > 0) {
            $this->valueError("Invalid float: Leading zeros not allowed", $s);
        }
        return $s;
    }

}

class TomPartTag
{
    public array $base;
    public int $n; // number of indexed arrays
    public bool $aot;
    public bool $implicit;

    function &endRef(): array
    {
        return $this->base[$this->n - 1];
    }

    function &newRef(): array
    {
        $ct = $this->n;
        $this->base[] = [];
        $this->n++;
        return $this->base[$ct];
    }


}
