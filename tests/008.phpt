--TEST--
Pluck UTF-8 character and code out of a string
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$s = "Hello world, Καλημέρα κόσμε, コンニチハ";
$code = 0;
$next = str_utf8c($s,13, $code);
echo $next . " "  . dechex($code) . PHP_EOL;
$next = str_utf8c($s,13 + strlen($next), $code);
echo $next . " "  . dechex($code) . PHP_EOL;
?>
--EXPECT--
Κ 39a
α 3b1