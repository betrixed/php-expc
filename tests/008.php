<?php
$s = new Csu32("Hello world, Καλημέρα κόσμε, コンニチハ" );
foreach($s as $ix => $val) {
	echo dechex($val) . " ";
}
echo PHP_EOL . $s . PHP_EOL;
?>
