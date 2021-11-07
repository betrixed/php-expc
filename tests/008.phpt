--TEST--
Create and iterator Csu32 from string
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$s = new Csu32("Hello world, Καλημέρα κόσμε, コンニチハ" );
foreach($s as $ix => $val) {
	echo "$ix: " . $val . PHP_EOL;
}
?>
--EXPECT--
