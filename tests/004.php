<?php
$s = new Str8("Hello world, Καλημέρα κόσμε, コンニチハ" );

echo $s . PHP_EOL;
$store = new CArray(CArray::CA_UINT32);

foreach($s as $key=>$value) {
	$store[] = $value;
}

foreach($store as $value) {
	echo dechex($value) . " ";
}

echo "unicode " . $store->size() . " capacity " . $store->capacity() PHP_EOL;