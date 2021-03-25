<?php
$s = new Str8("Hello world, Καλημέρα κόσμε, コンニチハ" );

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