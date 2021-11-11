<?php
include "utoken.php";
include "tomlparser.php";

	$p = new TomlParser();
	$data = file_get_contents(__DIR__ . "/fruit.toml");
	$r = $p->parse($data);
	echo print_r($r, true) . PHP_EOL;