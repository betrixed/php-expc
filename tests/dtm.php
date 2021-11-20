<?php

$s = "12:35:26.151502";

$value = daytime_str($s);
echo "float value = " . $value . PHP_EOL;
echo "time value = " . daytime_format($value,0) . PHP_EOL;

$tm = new DayTime($s);
echo "object = " . $tm . PHP_EOL;