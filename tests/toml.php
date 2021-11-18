<?php

$obj = new Ctoml();

$data = ["101", "1000_001", "_01000_001", "01000_001_"];

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
