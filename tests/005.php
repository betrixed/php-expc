<?php

function type_tests() 
{
$etypes = [CArray::CA_INT8, CArray::CA_UINT8, 
			CArray::CA_INT16, CArray::CA_UINT16,
			CArray::CA_INT32, CArray::CA_UINT32,
			CArray::CA_INT64, CArray::CA_UINT64
		  ];

$ftypes = [CArray::CA_REAL32, CArray::CA_REAL64];

foreach($etypes as $t => $e) {
	
	$z = new CArray($e, 100);
	echo "EType = " . $z->getTypeName() . " = " . $z->getType() . " : " . $z->getTypeSize() . PHP_EOL;
	foreach($z as $k => $v) {
		$newvalue = 10 * ($k << $z->getTypeSize());
		$z[$k] = $newvalue;
		//echo  "value = " . $newvalue  . "  fetched " . $z[$k]  . PHP_EOL;
	}
	echo PHP_EOL;

}

foreach($ftypes as $t => $e) {
	
	$z = new CArray($e, 100);
	echo "FType = " . $e . " = " . $z->getTypeName() . " : " . $z->getTypeSize() . PHP_EOL;
	foreach($z as $k => $v) {
		$newvalue = ($k << $z->getTypeSize()) ** M_PI;
		$z[$k] = $newvalue;
		//echo "value = " . $newvalue  . " fetched " . $z[$k] . PHP_EOL;
	}
	echo PHP_EOL;

}

}




type_tests();

function spl_test() {
	$mega = 1.0e-6;
	$mem_start = 0;
	$mem_end = $mem_start;

	$mem_start = memory_get_usage();
	$time_start = microtime(true);
	$spl = new SplFixedArray(100000);
	for($i = 0; $i < 100000; $i++) {
		$spl[$i] = $i;
	}
	$time_end = microtime(true);

	$mem_end = memory_get_usage();

	echo "memory spl: " .  ($mem_end - $mem_start) . PHP_EOL;
	echo "time spl: " .  ($time_end - $time_start) / $mega . PHP_EOL;
	$time_start = microtime(true);

	foreach($spl as $k => $v) {

	}
	$time_end = microtime(true);
	echo "2time spl: " .  ($time_end - $time_start) / $mega . PHP_EOL;}

function carray_test() {
	$mega = 1.0e-6;
	$mem_start = 0;
	$mem_end = $mem_start;
	$mem_start = memory_get_usage();
	$time_start = microtime(true);
	$spl = new CArray(CArray::CA_UINT64, 100000);
	for($i = 0; $i < 100000; $i++) {
		$spl->setValue($i,$i);
	}
	$time_end = microtime(true);
	$mem_end = memory_get_usage();

	echo "memory carray: " .  ($mem_end - $mem_start)  . PHP_EOL;
	echo "time carray: " .  ($time_end - $time_start) / $mega . PHP_EOL;
	$time_start = microtime(true);

	foreach($spl as $k => $v) {

	}
	$time_end = microtime(true);
	echo "2time carray: " .  ($time_end - $time_start) / $mega . PHP_EOL;
}


spl_test();
carray_test();

