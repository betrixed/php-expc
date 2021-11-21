<?php

$s = "12:35:26.151502";

$value = daytime_str($s);
echo "float value = " . $value . PHP_EOL;
echo "time value = " . daytime_format($value,0) . PHP_EOL;

$tm = new DayTime($s);
echo "object = " . $tm . PHP_EOL;

$value = daytime_set(18,53,12.5);

$tm->set(18,53,12.5);
echo "obj = " . $tm->format() . PHP_EOL;
echo "val = " . daytime_format($value,0) . PHP_EOL;

$hours = 0;
$mins = 0;
$secs = 0.0;

daytime_split($value,$hours,$mins,$secs);


echo "split fn = " . $hours . ":" . $mins . ":" . $secs . PHP_EOL;
$tm->split($hours,$mins,$secs);
echo "split obj = " . $hours . ":" . $mins . ":" . $secs . PHP_EOL;

try {
$tm->set(25,59,59.0);
}
catch(Exception $ex) {
	echo "Exception: " . $ex->getMessage() . PHP_EOL;;
}
try {
$tm->set(23,61,59.0);
}
catch(Exception $ex) {
	echo "Exception: " . $ex->getMessage() . PHP_EOL;;
}
try {
$tm->set(23,59,60.1);
}
catch(Exception $ex) {
	echo "Exception: " . $ex->getMessage() . PHP_EOL;;
}