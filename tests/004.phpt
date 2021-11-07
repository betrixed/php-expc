--TEST--
Test packed storage and iteration of CArray integer types
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
$etypes = [CArray::CA_INT8, CArray::CA_UINT8, 
			CArray::CA_INT16, CArray::CA_UINT16,
			CArray::CA_INT32, CArray::CA_UINT32,
			CArray::CA_INT64
		  ];

$ftypes = [CArray::CA_REAL32, CArray::CA_REAL64];


foreach($etypes as $t => $e) {
	$z = new CArray($e, 10);
	echo "EType = " . $z->getTypeName() . " = " . $z->getTypeEnum() . " : " . $z->getTypeSize() . PHP_EOL;
	foreach($z as $k => $v) {
		$newvalue = 10 * ($k << $z->getTypeSize());
		try {
		$z[$k] = $newvalue;
		echo  "value = " . $newvalue  . "  fetched " . $z[$k]  . PHP_EOL;
		}
		catch(Exception $ex) {
			echo "$newvalue outside array type range" . PHP_EOL;
		}
		
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
?>
--EXPECT--
EType = int8 = 2 : 1
value = 0  fetched 0
value = 20  fetched 20
value = 40  fetched 40
value = 60  fetched 60
value = 80  fetched 80
value = 100  fetched 100
value = 120  fetched 120
140 outside array type range
160 outside array type range
180 outside array type range

EType = uint8 = 3 : 1
value = 0  fetched 0
value = 20  fetched 20
value = 40  fetched 40
value = 60  fetched 60
value = 80  fetched 80
value = 100  fetched 100
value = 120  fetched 120
value = 140  fetched 140
value = 160  fetched 160
value = 180  fetched 180

EType = int16 = 4 : 2
value = 0  fetched 0
value = 40  fetched 40
value = 80  fetched 80
value = 120  fetched 120
value = 160  fetched 160
value = 200  fetched 200
value = 240  fetched 240
value = 280  fetched 280
value = 320  fetched 320
value = 360  fetched 360

EType = uint16 = 5 : 2
value = 0  fetched 0
value = 40  fetched 40
value = 80  fetched 80
value = 120  fetched 120
value = 160  fetched 160
value = 200  fetched 200
value = 240  fetched 240
value = 280  fetched 280
value = 320  fetched 320
value = 360  fetched 360

EType = int32 = 8 : 4
value = 0  fetched 0
value = 160  fetched 160
value = 320  fetched 320
value = 480  fetched 480
value = 640  fetched 640
value = 800  fetched 800
value = 960  fetched 960
value = 1120  fetched 1120
value = 1280  fetched 1280
value = 1440  fetched 1440

EType = uint32 = 9 : 4
value = 0  fetched 0
value = 160  fetched 160
value = 320  fetched 320
value = 480  fetched 480
value = 640  fetched 640
value = 800  fetched 800
value = 960  fetched 960
value = 1120  fetched 1120
value = 1280  fetched 1280
value = 1440  fetched 1440

EType = int64 = 16 : 8
value = 0  fetched 0
value = 2560  fetched 2560
value = 5120  fetched 5120
value = 7680  fetched 7680
value = 10240  fetched 10240
value = 12800  fetched 12800
value = 15360  fetched 15360
value = 17920  fetched 17920
value = 20480  fetched 20480
value = 23040  fetched 23040

FType = 32 = float : 4
value = 0 fetched 0
value = 6065.3307931275 fetched 6065.3305664062
value = 53526.409763233 fetched 53526.41015625
value = 191326.49707797 fetched 191326.5
value = 472369.37932353 fetched 472369.375
value = 952211.71935333 fetched 952211.75
value = 1688452.0944452 fetched 1688452.125
value = 2740364.2021735 fetched 2740364.25
value = 4168649.29872 fetched 4168649.25
value = 6035256.7292129 fetched 6035256.5

FType = 64 = double : 8
value = 0 fetched 0
value = 36788237.630061 fetched 36788237.630061
value = 324655381.3825 fetched 324655381.3825
value = 1160458494.2682 fetched 1160458494.2682
value = 2865076542.1416 fetched 2865076542.1416
value = 5775479062.9707 fetched 5775479062.9707
value = 10241020481.159 fetched 10241020481.159
value = 16621215379.828 fetched 16621215379.828
value = 25284236957.276 fetched 25284236957.276
value = 36605828484.125 fetched 36605828484.125

