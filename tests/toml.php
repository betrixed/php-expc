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

/*
$val = $obj->matchBase("0xDEADBEEF");
echo "Value = " . $val . " type = " . gettype($val) . PHP_EOL;

$data = ["01000_001_", "1000_001", "101" ,"_01000_001", "099"];

foreach( $data as $item) 
{
	try {
		$val = $obj->matchInt($item);
		echo "Value = " . $val . " type = " . gettype($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}




$data = ["1.01", "-99_99.001", "-0.010_001", "+0.018", "120.", "-.88"];
foreach( $data as $item) 
{
	try {
		$val = $obj->matchFloatDot($item);
		echo "Value = " . $val . " type = " . gettype($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}

$data = ["1.01e10", "99_99e-10", "-1e1", "+2.0e-3", "-120e14", "0.54e10", "1e3.1"];
foreach( $data as $item) 
{
	try {
		$val = $obj->matchFloatExp($item);
		echo "Value = " . $val . " type = " . gettype($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}


function isbool($val) : string {
	if ($val === true) {
		return "true";
	}
	else if ($val === false) {
		return "false";
	}
	else
		return "unknown";
}
$data = ["true", "false", "TRUE", "FALSE"];
foreach( $data as $item) 
{
	try {
		$val = $obj->matchBool($item);
		echo "Value = " . isbool($val)
			. " type = " . gettype($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}

$data = ["1979-05-27T07:32:00Z", "1979-05-27T00:32:00-07:00",
			"1979-05-27T00:32:00.999999-07:00",
			"1979-05-27T07:32:00", "1979-05-27T00:32:00.999999",
		"2021-10-31", "1979-05-27", "07:32:00"
		, "00:32:00.999999"];
foreach( $data as $item) 
{
	try {
		$val = $obj->matchDateTime($item);
		echo "Value = " . $val->format(DateTime::ATOM)
			. " type = " . get_class($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}
$data = ["21:53:01", "06:30","23:59:58.919193", "01:01:60.0"];

foreach( $data as $item) 
{
	try {
		$val = $obj->matchTime($item);
		echo "Value = " . $val->format()
			. " type = " . get_class($val) . PHP_EOL;
	} 
	catch (Exception $ex) {
		echo $ex->getMessage() . PHP_EOL;
	}
}
*/

