<?php
/** test the concepts, with php 8.0+ , especially the regular expressions implementation */
class TomlParser {
	const tom_EOS = 0;
	const tom_Equal = 1;
	const tom_Bool = 2;
	const tom_DateTime = 3;
	const tom_FloatExp = 4;
	const tom_FloatDot = 5;
	const tom_Float_E = 6;
	const tom_Integer = 7; //5
	const tom_Quote3 = 8;
	const tom_Quote1 = 9;
	const tom_Apost3 = 10;
	const tom_Apost1 = 11;
	const tom_NewLine = 12; // 10
	const tom_Space = 13;
	const tom_LSquare = 14; //12
	const tom_RSquare = 15;
	const tom_LCurly = 16;
	const tom_RCurly = 17;
	const tom_Comma = 18;
	const tom_Dot = 19;
	const tom_BareKey = 20; // 18
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

 	protected array  $root;			// root array
 	protected array  $table_paths; // parallel name paths for name system tags.
 	protected array  $name_stack; // path parts to current table;

 	protected array  $table;  // reference original
 	protected int    $index;
 	protected string $hold;
 	protected int    $slen;
 	protected array  $map;
	protected bool   $flagLF;
	protected array  $expSet;
	protected int    $tokenLine;

	protected array $key_exp = [self::tom_Space, self::tom_BareKey, self::tom_Integer];
	protected array $value_exp = [self::tom_Bool, self::tom_DateTime, self::tom_FloatExp, self::tom_FloatDot];
	protected array $estr_exp = [self::tom_Space, self::tom_NoEscape, self::tom_EscapedChar, self::tom_Quote3];
	protected array $lstr_exp = [self::tom_LitString];
	protected array $ml_exp = [self::tom_LitString, self::tom_Apost3];
	protected array $stack = [];

 	static function initMap() : array {
 		$exp = [ 
	 		self::tom_Bool => '(true|false)',
	 		self::tom_DateTime => '(\d{4}-\d{2}-\d{2}(T\d{2}:\d{2}:\d{2}(\.\d{6})?(Z|-\d{2}:\d{2})?)?)',

	 		self::tom_FloatExp => '([+-]?((\d_?)+([\.](\d_?)*)?)([eE][+-]?(_?\d_?)+))',
			self::tom_FloatExp => '([+-]?((\d_?)+([\.](\d_?)*)?)([eE][+-]?(_?\d_?)+))',
			self::tom_FloatDot => '([+-]?((\d_?)+([\.](\d_?)*)))',
			self::tom_Integer => '([+-]?(\d_?)+)',

			self::tom_Quote3 => '(\"\"\")',
			self::tom_Apost3 => '(\'\'\')',
			self::tom_Space => '(\h+)',
			self::tom_BareKey => '([-A-Z_a-z0-9]+)',

			self::tom_EscapedChar => '(\\(n|t|r|f|b|\\"|\\|u[0-9A-Fa-f]{4,4}|U[0-9A-Fa-f]{8,8})',
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
		foreach($exp as $ix => $val) {
			$map[$ix] = "/" . $val . "/u";
		}
		return $map;
 	}

 	public function firstMatch() : array {
 		foreach($this->expSet as $id) {
 			$exp = $this->map[$id];
 			$match = null;
 			$res = preg_match($exp, $this->hold, $match, 0, $this->index);
 			if (!empty($match)) {
 				return [$id,$match];
 			}
 		}
 		return [0,null];

 	}

 	/**
 	 * Update token with next character and code in subject 
 	 */
 	public function peekChar(UToken $token) : int {
 		if ($this->index < $this->slen) {
 			$code = 0;
 			$value = str_utf8c($this->hold,$this->index,$code);
 			if ($code !== -1) {
 				$token->code = $code;
 				$token->value = $value;
 				return strlen($value);
 			}
 			$token->id = self::tom_EOS;
 			$token->code = -1;
 			$token->value = '';
 			$token->isSingle = true;
 			return 0;
 		}
 	}

 	public function checkLF(UToken $token) {
 		if ($token->code === 13) {
 			$this->index += 1;
 			$this->peekChar($token);
 		}
 		if ($token->code === 10) {
 			$this->flagLF = true;
 			$token->id = self::tom_NewLine;
 			$token->line = $this->tokenLine;
 			$token->isSingle = true;
 			return;
 		}
 		throw new Exception("Invalid character code: " . dechex($token->code));
 	}

 	public function peekToken(UToken $token) {
 		$csize = $this->peekChar($token);
 		if ($csize === 0) {
 			$token->line = $this->tokenLine;
 		}
 		else if ($token->code < 20) {
 			$this->checkLF();
 		}
 		else {
 			$token->line = $this->tokenLine;
 			$token->isSingle = false;
 			if ($csize === 1) {
 				$fit = $this->singleMatch($token->code);
 				if ($fit) {
 					$token->id = $fit;
 					$token->isSingle = true;
 				}
 				else {
 					$token->id = self::tom_AnyChar;
 				}
 			}
 			else {
 				$token->id = self::tom_AnyChar;
 			}
 		}
 	}

 	public function singleMatch(int $code) : int {
 		switch(code) {
		case ord('='): return self::tom_Equal;
		case ord('['): return self::tom_LSquare;
		case ord(']'): return self::tom_RSquare;
		case ord('.'): return self::tom_Dot;
		case ord(','): return self::tom_Comma;
		case ord('{'): return self::tom_LCurly;
		case ord('}'): return self::tom_RCurly;
		case ord('\''): return self::tom_Apost1;
		case ord('\"'): return self::tom_Quote1;
		case ord('#'): return self::tom_Hash;
		case ord('\\'): return self::tom_Escape;
		case ord(' '): return self::tom_Space;
		case ord('\t'): return self::tom_Space;
		case ord('\f'): return self::tom_Space;
		case ord('\b'): return self::tom_Space;
		default: return 0;
		}
	}

	public function moveNext() : int {
		$token = $this->token;
		if ($this->flagLF) {
			$this->flagLF = false;
			$this->tokenLine += 1;
			$token->line = $this->tokenLine;
		}
		$nextCt = $this->peekChar($token);
		if ($nextCt===0) {
			$this->checkLF($token);
			$this->index++;
		}
		else {
			[$id, $capture] = $this->firstMatch();
			if ($id > 0 && count($capture) > 1) {
				$token->id = $id;
				$token->value = $capture[1];
				$token->isSingle = false;
				$this->index += strlen($capture[1]);
			}
			else {
				if ($id > 0) {
					throw new Exception('Bad capture array');
				}
				$this->index += $nextCt;
				$token->isSingle = false;
				if ($nextCt === 1) {
					$fit = $this->singleMatch($token->ucp);
					if ($fit) {
						$token->id = $fit;
						$token->isSingle = true;
					}
					else {
						$token->id = self::tom_EOS;
					}
				}
				else {
					$token->id = self::tom_EOS;
				}
			}
		}
		$token->line = $this->tokenLine;
		return $token->id;
	}

	function moveExpr(int $id) : bool
	{
		$expr = $this->map[$id];
		if ($this->index < $this->slen) {
			// rematch for a chosen expression
			$match = null;
			$res = preg_match($this->map[$id], $this->hold, $match, 0, $this->index);
			if (is_integer($res) && count($match)>1) {
				$token = $this->token;
				$token->value = $match[1];
				$next = count($token->value);
				$this->index += $next;
				$token->id = self::tom_AnyChar;
				$token->isSingle = false;
				return true;
			}
		}
		return false;
    }

    function parseComment() : bool
    {
    	return $this->moveExpr(self::tom_Comment);
    }


    function integer() : string {
    	$s = $this->token->value;
    	if ((substr($s,-1)==="_") || (substr($s,0,1)==="_"))
		{
			$this->valueError("Invalid integer: Underscore must be between digits", $s);
		}

    	return strreplace('_','',$s);
    }

    function popExpSet(array $set) {
    	if (!empty($this->stack)) {
    		$this->expSet = array_pop($this->stack);
    	} 
    	else {
    		$this->syntaxError("Expression stack error");
    	}
    }

    function pushExpSet(array $set) {
    	$this->stack[] = $this->expSet;
    	$this->expSet = $set;
    }

    function escape_string(string $s) : string {
    	$val = substr($s,1,1);
    	switch($val) {
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
    			return IntlChar::chr(hexdec(substr($s,2,4)));
    		case 'U':
    			return IntlChar::chr(hexdec(substr($s,2,8)));
    		default:
    			$this->svalueError("Invalid ecape character ", $val);
    			break;
    	}
    	return '';
    }
    function quotedStr() : string {
    	$this->pushExpSet($this->$estr_exp);
    	$id = $this->moveNext();
    	$s = '';
    	while ($id != self::tom_Quote1) {

    		if (in_array($id, [self::tom_NewLIne, self::tom_EOS, self::tom_Escape], true)) {
    			$this->syntaxError("Incomplete string value");
    		}
    		else if ($id === self::tom_EscapedChar) {
    			$s .= $this->escape_string($this->token->value);

    		}
    		else {
    			$s .= $this->token->value;
    		}
    	}
    	$this->popExpSet();
    	return $s;
    }

    function doneToken(UToken $token) {
    	if ($this->flagLF) {
    		$this->flagLF = false;
    		$this->tokenLine += 1;
    	}
    	$token->line = $this->tokenLine;
    	if ($token->id === self::tom_EOS) {
    		return;
    	}
    	if ($okten->id === self::tom_NewLine) {
    		$this->index++;
    		return;
    	}
    	$this->index += strlen($token->value);
    }
    function literalStr() : string {
    	$ret = '';
    	$token = $this->token;
    	$this->peekToken($token);
    	while($token->id !== self::tom_Apost1) {
	    	if ($token->id === self::tom_NewLine || $token->id === self::tom_EOS) {
	    		$this->syntaxError("Value without end quote (')");
	    	}
	    	if ($this->moveExpr(self::tom_LitString)) {
	    		$ret .= $token->value;
	    	}
	    	else {
	    		$this->syntaxError("Invalid string value");
	    	}
	    	$this->peekToken($token);
    	}
    	$this->doneToken($token);
    }

    function syntaxError(string $msg) {
    	throw new Exception("TOML error: " . $msg);
    }

    function valueError(string $msg, string $value) {
    	throw new Exception($msg . " Value { $value }");
    }
	function keyName() : string
	{
		$token = $this->token;
		switch($token->id) {
			case self::tom_BareKey: 
				return $token->value;
			case self::tom_Quote1:
				return $this->quotedStr();
			case self::tom_Apost1:
				return $this->literalStr(name);
			case self::tom_Integer:
				return $this->integer();
			default:
				$this->syntaxError("Improper Key");
				break;
		};
		return '';
	}

	function listSame() : array 
	{
		$token = $this->token;
		$ret = [];
		$inloop = true;
		$val0 = null;
		while($inloop) {
			$this->peekToken($token);
			switch($token->id) {
				case self::tom_Space:
					$this->moveExpr(self::tom_Space);
					break;
				case self::tom_NewLine:
					$this->doneToken($token);
					break;
				case self::tom_Hash:
					$this->doneToken($token);
					$this->comment();
					break;
				case self::tom_EOS:
					$this->doneToken($token);
					$this->syntaxError("Missing ']'");
					break;
				case self::tom_RSquare:
					$this->doneToken($token);
				default:
					$inloop = false;
					break;
			}
		}
		while ($token->id !== self::tom_RSquare) {
			if ($token->id == self::tom_LSquare) {
				$this->doneToken($token);
				if (count($ret) === 0) {
					$val0 = 'array';
				}
				else {
					if ($val0 !== 'array') {
						$this->arrayTypeError($val0, 'array');
					}
				}
				$ret[] = $this->listSame();
			}
			else {
				$value = $this->value();
				if (count($ret)===0) {
					$val0 = gettype($value);
				}
				else {
					$valn = gettype($value);
					if ($valn != $val0) {
						$this->arrayTypeError($valn, $valn);
					}
				}
				$ret[] = $value;
			}
		}
		$inloop = true;
		$comma = false;
		while($inloop) {
			$this->peekToken($token);
			switch($token->id) {
				case self::tom_Space:
					$this->moveExpr(self::tom_Space);
					break;
				case self::tom_NewLine:
					$this->doneToken($token);
					break;
				case self::tom_Hash:
					$this->doneToken($token);
					$this->comment();
					break;
				case self::tom_Comma:
					if ($comma) {
						$this->syntaxError('No value between commas');
					}
					$comma = true;
					$this->doneToken($token);
					break;
				case self::tom_EOS:
					$this->doneToken($token);
					$this->syntaxError("Missing ']'");
					break;
				case self::tom_RSquare:
					$this->doneToken($token);
				default:
					$inloop = false;
					break;
			}
		}
		return $ret;
	}

	function inlineTable() : void
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
		while($id === self::tom_Comma) {
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

	function arrayMatchError(string $a, string $b) {
		throw new Exception("Lists must be of same type: $a <> $b");
	}

	function keyValue() {
		$key = $this->keyName();
		if (isset($this->table[$key])) {
			$this->syntaxError("Duplicate key: $key");
		}	
		if (!$this->moveExpr(self::tom_SpacedEqual) )
		{
			$this->syntaxError("Expect <key> = ");
		}
		$id = $this->token->id;
		$rhs = null;
	
		if ($id === self::tom_LSquare) {
			$this->doneToken($token);
			$rhs = $this->listSame();
		}
		else if ($id === self::tom_LCurly) {
			$this->doneToken($token);
			$oldTable = &$this->table;
			$rhs = [];
			$this->table = &$rhs;
			$this->inlineTable();
			$this->table = &$oldTable;
		}
		else {
			$rhs  = $this->value();
		}
		$this->table[$key] = $rhs;
	}

	function finishLine() : int
	{
		$this->moveExpr(self::tom_HashComment);
		$ret = $this->moveNext();
		if ($ret !== self::tom_Newline && $ret !== self::tom_EOS) {
			syntaxError("Expect end line or end document");
		}
		return $ret;
	}

	function parse(string $src) : array {
		$this->map = self::initMap();
		$this->hold = $src;
		$this->index = 0;
		$this->slen = strlen($src);
		$this->flagLF = false;
		$this->tokenLine = 0;
		$this->token = new UToken();
		$this->root = [];
		$this->table = &$this->root;
		$this->expSet = $this->key_exp;

		$id = $this->moveNext();
		while ($id !== self::tom_EOS) {
			switch($id) {
				case self::tom_Hash:
					$this->parseComment();
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
		return $this->root;
	}
	// make a descriptive path for generating an exception
	function join_parts(bool $withIndex = true) : string
	{
		$ret = '';

		foreach($this->name_stack as $ix => $part)
		{
			if ($ix > 0) {
				$ret .= '.';
			}
			$ret .= 
			$tag = $this->table_paths[$path];
			if ($tag->aot) {
				$ret .= "[" . $part;
				if ($withIndex) {
					$ret .= count($tag->base) - 1;
				}
				$ret .- "]";
			}
			else {
				$ret .= "{" . $part . "}";
			}
		}
		return $ret;
	}

	function tablePath()
	{
		$partName = '';
		$isAOT = false;
		$hitNew = false;
		$parts = [];
		$dotCount = 0;
		$aotLength = 0;
		$path_ref = &$this->root;


		$id = $this->moveNext();
		$inloop = true;

		while($inloop) {
			switch($id) {
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
				case self::tom_Newline:
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
					}
					else {
						$inloop = false;
					}
					break;
				case self::tom_LSquare:
					if ($dotCount < 1 && count($parts) > 0) {
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
					if ($dotCount < 1 && !empty($parts)) {
						$this->syntaxError("Expected a '.' after path part");
					}
					$otag = new TomPartTag();
					$otag->aot = $isAOT;
					$part_key = $this->keyName();
					$otag->part = $part_key;
					
					$dotCount = 0;
					$path_here = $this->join_parts(true);

					$new_path = $path_here . "." . $part_key;

					if (array_key_exists($part_key, $path_ref)) {
						$leaf = &$path_ref[$part_key];
					
						// There's a key-value stored already

						if (!is_array($leaf)) {
							// its a value and we want to set an object
							$this->syntaxError("Duplicate key path: " 
								. $new_path);
						}
						$tag = $this->table_paths($path_here);

						if ($tag->aot) {
							$aotLength++;
						}
					}
					else {
						// make new branch
						if (!$hitNew) {
							$hitNew = true;
						}
						// first branch declaration has to be correct
						if ($isAOT) {
							$aotLength++;	
						}
						$leaf = [];
						$path_ref[$part_key] = &$leaf;
						$otag->implicit = true;
						$this->table_parts[$new_path] = $otag;
					}
					$otag->base = &$leaf;
					$path_ref = &$leaf;
					$parts[] = $otag;
					$id = $this->moveNext();
					break;
			} // end switch

			if (empty($parts)) {
				$this->syntaxError("Table path cannot be empty");
			}
			if (!$hitNew) {
				// check last object && last parts value
				$otag = $parts[count($parts)-1];
				$path_here = $this->join_parts(true);

				$tag = $this->table_parts[$path_here];

				if ($tag->aot) {
					if ($otag->aot) {
						$leaf = [];
						$path_ref[$part_key] = &$leaf;
					}
					else {
						$this->syntaxError("Table path mismatch with " . $this->join_parts(false));
					}
				}
				else {
					// Table part created earlier, allow if it was just implicit creation
					if ($tag->implicit) {
						$tag->implicit = false; // won't allow this to happen again
					}
					else {
						$this->syntaxError("Duplicate key path: " . $this->join_parts(false));
					}
				}
			}
			else {
				$tag = $this->table_parts[$new_path];
				$tag->implicit = false; // end of path cannot be implicit
			}
			$this->table = &$leaf;
		} // end while
	} // end function
}

class TomlPartTag {
	public string $part;
	public array $base;
	public bool  $aot;
	public bool  $implicit;
}