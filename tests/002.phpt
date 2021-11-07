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
array(2) {
  [0]=>
  string(15) "/series/([^/]*)"
  [1]=>
  array(1) {
    ["id"]=>
    int(1)
  }
}
