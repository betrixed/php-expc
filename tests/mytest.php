<?php
if (!class_exists('Cregx')) {
	echo "Not found" . PHP_EOL;
	die;
}
$re = new Cregx('/\G\w*\s*(\w*)/',0,1);
$ret = $re->match("Whats up doc",0);
if ($ret) {
	echo 'return ' . print_r($ret,true) . PHP_EOL;
	$captures = $re->captures();
	$re->clear();
	echo print_r($captures,true) . PHP_EOL;
	$ret = $re->match("Hello world to re",0);
	echo 'return ' . print_r($ret,true) . PHP_EOL;
	$captures = $re->captures();
	$re->clear();
	echo print_r($captures,true) . PHP_EOL;
	echo 'END' . PHP_EOL;
	var_dump($captures);
}
?>
