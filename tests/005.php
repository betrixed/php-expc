<?php

echo "etype = " . CArray::CARRAY_UINT32 . PHP_EOL;

$a = new CArray(CArray::CARRAY_UINT32,10);

echo "count = " . count($a) . PHP_EOL;

$a->offsetSet(0,1);
echo "0: " . $a[0] . PHP_EOL;

$a[9] = 100;
echo "9: " . $a[9] . PHP_EOL;

echo "etype = " . CArray::CARRAY_INT8 . PHP_EOL;

$b = new CArray(CArray::CARRAY_INT8,12);

echo "count = " . count($b) . PHP_EOL;

$b->offsetSet(0,1);
echo "0: " . $b[0] . PHP_EOL;

$b[11] = 127;
echo "11: " . $b[11] . PHP_EOL;
$b[10] = -128;
echo "10: " . $b[10] . PHP_EOL;

$d = new CArray(CArray::CARRAY_UINT32,0);
echo "count = " . count($d) . PHP_EOL;
$d->setSize(100);
echo "count = " . count($d) . PHP_EOL;


$e = $d;

echo "e count = " . count($e) . PHP_EOL;
$e[0] = 12;

echo "d[0] =  " . $d[0] . PHP_EOL;

foreach($e as $key => $val) {
	echo $key . " " . $val . PHP_EOL;
}