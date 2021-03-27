<?php
$test = ["value1"];
debug_zval_dump($test);
$test = ["value1", ["id" => 1]];
debug_zval_dump($test);


$ret = route_extract_params("/series/{id}");
debug_zval_dump($ret);


function pf_extractParams(string $pattern): ?array {
    $prevChp = 0;
    $bracketCount = 0;
    $parenthesesCount = 0;
    $foundPattern = 0;
    $intermediate = 0;
    $numberMatches = 0;

    if (strlen($pattern) === 0) {
        return null;
    }

    $cp_underscore = ord('_');
    $cp_dash = ord('-');
    $cp_colon = ord(':');
    $cp_lbt = ord('{');
    $cp_rbt = ord('}');
    $cp_lps = ord('(');
    $cp_rps = ord(')');
    $cp_bsl = ord('\\');

    $matches = [];
    $route = "";
    $notValid = false;
    // provide array of UTF8 characters
    $chars = preg_split('//u', $pattern, null, PREG_SPLIT_NO_EMPTY);
    foreach ($chars as $cindex => $ch) {
        $ch_pt = IntlChar::ord($ch);
        if ($parenthesesCount === 0) {
            if ($ch_pt === $cp_lbt) {
                if ($bracketCount === 0) {
                    $marker = $cindex + 1;
                    $intermediate = 0;
                    $notValid = false;
                }
                $bracketCount++;
            } elseif ($ch_pt === $cp_rbt) {
                $bracketCount--;
                if ($intermediate > 0) {
                    if ($bracketCount === 0) {
                        $numberMatches++;
                        $variable = null;
                        $regexp = null;
                        // the substring is an  slice of $chars array
                        $item = array_slice($chars, $marker, $cindex - $marker);
                        $item_str = implode('', $item); // need this later
                        foreach ($item as $cursorVar => $chv) {
                            $cpt = IntlChar::ord($chv);
                            if ($cpt === 0) { // how did \0 get here?
                                break;
                            }
                            if (($cursorVar === 0) && !IntlChar::isalpha($cpt)) {
                                $notValid = true;
                                break;
                            }
                            if (IntlChar::isalnum($cpt) ||
                                    $cpt === $cp_dash || $cpt === $cp_underscore || $cpt === $cp_colon) {
                                if ($cpt === $cp_colon) {
                                    $variable = implode('', array_slice($item, 0, $cursorVar));
                                    $regexp = array_slice($item, $cursorVar + 1);
                                    break;
                                }
                            } else {
                                $notValid = true;
                                break;
                            }
                        }

                        if (!$notValid) {
                            $tmp = $numberMatches;
                            if (!empty($variable) && !empty($regexp)) {
                                $foundPattern = 0;
                                foreach ($regexp as $ch) {
                                    $cpt = IntlChar::ord($ch);
                                    if ($cpt === 0) {
                                        break;
                                    }
                                    if (!$foundPattern) {
                                        if ($cpt === $cp_lps) {
                                            $foundPattern = 1;
                                        }
                                    } else {
                                        if ($cpt === $cp_rps) {
                                            $foundPattern = 2;
                                            break;
                                        }
                                    }
                                }

                                $rxstr = implode('', $regexp);
                                if ($foundPattern != 2) {
                                    $route .= "(" . $rxstr . ")";
                                } else {
                                    $route .= $rxstr;
                                }
                                $matches[$variable] = $tmp;
                            } else {
                                $route .= "([^/]*)";
                                $matches[$item_str] = $tmp;
                            }
                        } else {
                            $route .= "{" . $item_str . "}";
                        }
                        continue;
                    }
                }
            }
        }

        if ($bracketCount === 0) {
            if ($ch_pt === $cp_lps) {
                $parenthesesCount++;
            } elseif ($ch_pt === $cp_rps) {
                $parenthesesCount--;
                if ($parenthesesCount === 0) {
                    $numberMatches++;
                }
            }
        }

        if ($bracketCount > 0) {
            $intermediate++;
        } else {
            if (($parenthesesCount === 0) && ($prevChp !== $cp_bsl)) {
                if (strpos(".+|#", $ch) !== false) {
                    $route .= '\\';
                }
            }
            $route .= $ch;
            $prevChp = $ch_pt;
        }
    }
    return [$route, $matches];
}

$ret = pf_extractParams("/series/{id}");
debug_zval_dump($ret);

function test_pf(int $rpt) {
	$time_start = microtime(true);
	for($i=0; $i < $rpt; $i++) {

		$result = pf_extractParams("/series/{id}");
	}
	$time_stop = microtime(true);
	return ($time_stop - $time_start) * 1000;
}

function test_ext(int $rpt) {
	$time_start = microtime(true);
	for($i=0; $i < $rpt; $i++) {

		$result = route_extract_params("/series/{id}");
	}
	$time_stop = microtime(true);
	return ($time_stop - $time_start) * 1000;
}

$rpt = 100;
$pf = test_pf($rpt );
$ef = test_ext($rpt );

echo "ratio " . $pf / $ef . PHP_EOL;
echo "pf ms time per $rpt " . $pf . PHP_EOL;
echo "ef ms time per $rpt " . $ef . PHP_EOL;