<?php
$x = 1;
while ($x < PHP_INT_MAX) {
	if ($x <= 256) {
		$x *= 2;
	}
	else {
		$x *= 1.61803;
		if (is_nan($x)) {
			break;
		}
		if ($x >= PHP_INT_MAX) {
			break;
		}
		$x = intval(ceil($x));
	
	}
	echo $x . ", ";
}
