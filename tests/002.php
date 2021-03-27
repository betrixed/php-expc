<?php
$test = ["value1", ["id" => 1]];
debug_zval_dump($test);

$ret = route_extract_params("/series/{id}");
debug_zval_dump($ret);