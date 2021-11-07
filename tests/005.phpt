--TEST--
Check zval storage and storage limit values
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php

function mixed_tests() {

	$a = new CArray(CArray::CA_MIXED);
	echo "New" . PHP_EOL;
	$a[] = "Hello zval ";
	$a[] = "World";
	foreach($a as $key => $value) {
		echo $value;
	}
	echo PHP_EOL;
	echo $a[0] . $a[1] . PHP_EOL;
	unset($a[0]);
	echo $a[0] . $a[1] . PHP_EOL;
}
function type_tests() 
{
	echo "PHP_INTMAX = " . PHP_INT_MAX . PHP_EOL;

	$etypes = [CArray::CA_INT8, CArray::CA_UINT8, 
			CArray::CA_INT16, CArray::CA_UINT16,
			CArray::CA_INT32, CArray::CA_UINT32,
			CArray::CA_INT64
		  ];


	$limits = [
				CArray::CA_INT8 => [-1*intval(ceil(2**7)), intval(ceil(2**7-1))],
				CArray::CA_UINT8 => [0, intval(ceil(2**8-1))],
				CArray::CA_INT16 => [-1 * intval(ceil(2**15)), intval(ceil(2**15-1))],
				CArray::CA_UINT16 => [0, intval(ceil(2**16-1)) ],
				CArray::CA_INT32 => [ -1*intval(ceil(2**31)), intval(ceil(2**31-1)) ],
				CArray::CA_UINT32 => [ 0, intval(ceil(2**32-1)) ],
				CArray::CA_INT64 => [ -PHP_INT_MAX-1, PHP_INT_MAX ]
		  ];
	$ftypes = [CArray::CA_REAL32, CArray::CA_REAL64];

		foreach($etypes as $t => $e) {
				$z = new CArray($e);
				$etype = $z->getTypeEnum();
				$ename = $z->getTypeName();

				echo "EType = " . $ename . " Enum = " . $etype . PHP_EOL;
				echo ($z->signedType() ? "signed" : "unsigned") . $z->getTypeSize()*8 .  " " ;
			
				for($i = 0; $i < 2; $i++) {
				try {
				
				$eset = $limits[$etype];
				$z[] = $eset[$i];
				}

		catch (\Exception $ex) {
			echo $ex->getMessage() . PHP_EOL;
		}

		}
		foreach($z as $val) {
			echo $val . " ";
		}
		echo PHP_EOL;
	}

	foreach($ftypes as $t => $e) {
		$z = new CArray($e, 10);
		echo "EType = " . $z->getTypeName() . " = " . $z->getTypeEnum() . PHP_EOL;
		echo ($z->signedType() ? "signed" : "unsigned") . $z->getTypeSize()*8 .  PHP_EOL;
		foreach($z as $k => $v) {
			$newvalue = (($k*10) << $z->getTypeSize()) ** M_PI;
			$z[$k] = $newvalue;
		}
	}
}

mixed_tests();
type_tests();
?>
--EXPECT--
New
Hello zval World
Hello zval World
World
PHP_INTMAX = 9223372036854775807
EType = int8 Enum = 2
signed8 -128 127 
EType = uint8 Enum = 3
unsigned8 0 255 
EType = int16 Enum = 4
signed16 -32768 32767 
EType = uint16 Enum = 5
unsigned16 0 65535 
EType = int32 Enum = 8
signed32 -2147483648 2147483647 
EType = uint32 Enum = 9
unsigned32 0 4294967295 
EType = int64 Enum = 16
signed64 -9223372036854775808 9223372036854775807 
EType = float = 32
signed32
EType = double = 64
signed64
