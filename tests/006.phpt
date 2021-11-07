--TEST--
Test str_camel and str_uncamel
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$test = str_camel("hello_world");

echo $test . PHP_EOL;

$snake = str_uncamel($test);

echo $snake . PHP_EOL;
?>
--EXPECT--
HelloWorld
hello_world
