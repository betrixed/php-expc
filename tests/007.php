<?php

$a = route_extract_params("/series/{id}");

echo print_r($a,true) . PHP_EOL;

$test = str_camel("hello_world");

echo $test . PHP_EOL;

$snake = str_uncamel($test);

echo $snake . PHP_EOL;