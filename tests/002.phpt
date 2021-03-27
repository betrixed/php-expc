--TEST--
test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$ret = route_extract_params("/series/{id}");
var_dump($ret);
?>
--EXPECT--
The extension phiz is loaded and working!
NULL
