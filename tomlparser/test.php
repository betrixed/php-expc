<?php

include "tomlparser.php";

	$p = new TomlParser();
        foreach( [
            'fruit.toml', 
            'example.toml', 
            'hard_example.toml', 
            'hard_example_unicode.toml'] 
                as $name) {
            $data = file_get_contents(__DIR__ . "/" . $name);

            $r = $p->parse($data);

            echo print_r($r, true) . PHP_EOL;
       }