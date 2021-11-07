--TEST--
CArray sub slice creation
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php
	$c = new CArray(CArray::CA_UINT32,10);
	for($i = 0; $i < 10; $i++) {
		$c[$i] = 2*$i;
	}
	$d = $c->sub(5,5);
	foreach($d as $ix => $val) {
		echo $ix . "=" . $val . PHP_EOL;
	}
?>
--EXPECT--
0=10
1=12
2=14
3=16
4=18
