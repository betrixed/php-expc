<?php
$s = new Str8("Hello world, Καλημέρα κόσμε, コンニチハ" 
);
foreach($s as $key=>$value) {
	echo $key . "  " . dechex($value) . "  " . $s->byteslen() . " " . $s->bytes() . PHP_EOL;
}

