<?php

$etypes = [CArray::CA_INT8, CArray::CA_UINT8, 
			CArray::CA_INT16, CArray::CA_UINT16,
			CArray::CA_INT32, CArray::CA_UINT32,
			CArray::CA_INT64, CArray::CA_UINT64
		  ];

$ftypes = [CArray::CA_REAL32, CArray::CA_REAL64];

foreach($etypes as $t => $e) {
	
	$z = new CArray($e, 10);
	echo "EType = " . $z->getTypeName() . " = " . $z->getType() . " : " . $z->getTypeSize() . PHP_EOL;
	foreach($z as $k => $v) {
		$newvalue = 10 * ($k << $z->getTypeSize());
		$z[$k] = $newvalue;
		echo  "value = " . $newvalue  . "  fetched " . $z[$k]  . PHP_EOL;
	}
	echo PHP_EOL;

}

foreach($ftypes as $t => $e) {
	
	$z = new CArray($e, 10);
	echo "FType = " . $e . " = " . $z->getTypeName() . " : " . $z->getTypeSize() . PHP_EOL;
	foreach($z as $k => $v) {
		$newvalue = ($k << $z->getTypeSize()) ** M_PI;
		$z[$k] = $newvalue;
		echo "value = " . $newvalue  . " fetched " . $z[$k] . PHP_EOL;
	}
	echo PHP_EOL;

}
