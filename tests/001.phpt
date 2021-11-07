--TEST--
Check if phiz is loaded
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "phiz" is available' . PHP_EOL;
?>
--EXPECT--
The extension "phiz" is available
