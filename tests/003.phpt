--TEST--
Iterate UTF-8 php string type
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$s = new Csu8("Hello world, Καλημέρα κόσμε, コンニチハ" );
echo $s . PHP_EOL;
$store = new CArray(CArray::CA_UINT32);
$offsets = new CArray(CArray::CA_UINT32);

foreach($s as $key=>$value) {
	$store[] = $value;
	$offsets[] = $key;
}
foreach($offsets as $value) {
	echo $value . " ";
}
echo PHP_EOL;
foreach($store as $value) {
	echo dechex($value) . " ";
}
echo PHP_EOL;
echo  $store->size() . " unicode characters,  buffer size " . $store->capacity() . PHP_EOL;
?>
--EXPECT--
Hello world, Καλημέρα κόσμε, コンニチハ
1 2 3 4 5 6 7 8 9 10 11 12 13 15 17 19 21 23 26 28 30 31 33 36 38 40 42 43 44 47 50 53 56 59 
48 65 6c 6c 6f 20 77 6f 72 6c 64 2c 20 39a 3b1 3bb 3b7 3bc 1f73 3c1 3b1 20 3ba 1f79 3c3 3bc 3b5 2c 20 30b3 30f3 30cb 30c1 30cf 
34 unicode characters,  buffer size 64