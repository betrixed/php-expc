--TEST--
Parse mix of TOML files
--SKIPIF--
<?php
if (!extension_loaded('phiz')) {
    echo 'skip';
}
?>
--FILE--
<?php

$obj = new Ctoml();

$data_path = dirname(__DIR__);

$parseFiles = [
	'fruit.toml', 
	'example.toml', 
	'hard_example.toml', 
	'hard_example_unicode.toml'
];

function tree_count(array $data) : int {
	$ct = count($data);
	foreach($data as $item) {
		if (is_array($item)) {
			$ct += tree_count($item);
		}
	}
	return $ct;
}
foreach($parseFiles as $name) {
	$td = file_get_contents($data_path . "/tests/" . $name);
	try {
		$data = $obj->parse($td);
		echo "Total keys = " . tree_count($data) . PHP_EOL;
	}
	catch(Exception $e){
		echo "Caught error: " . $e->getMessage() 
		. PHP_EOL . "File " . $name . PHP_EOL;
	}
}
?>
--EXPECT--
Total keys = 12
Total keys = 71
Total keys = 15
Total keys = 15
