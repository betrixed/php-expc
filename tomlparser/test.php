<?php

include "tomlparser.php";

/** 
 * A simple visual check
 */
class MapOutput
{
    private array $pathStack = [];

    function aout(array $data)
    {

        foreach ($data as $key => $value) {

            if (is_array($value)) {
                $this->pathStack[] = $key;
                $title = "[";
                foreach ($this->pathStack as $ix => $key) {
                    if ($ix > 0) {
                        $title .= ".";
                    }
                    $title .= $key;
                }
                $title .= "]";
                echo $title . PHP_EOL;
                $this->aout($value);
                continue;
            }
            if (is_object($value)) {
                switch (get_class($value)) {
                    case "DateTime":
                        $s = $value->format(DateTime::W3C);
                        break;
                    case "DayTime":
                        $s = $value->format();
                        break;
                }
                echo $key . " = " . $s . PHP_EOL;
                continue;
            }
            if (is_string($value)) {
                echo $key . " = " . "\"" . addslashes($value) . "\"" . PHP_EOL;
            }
        }
        if (!empty($this->pathStack)) {
            array_pop($this->pathStack);
        }
    }

}

function tree_count(array $data) : int 
{
    $ct = count($data);
    foreach($data as $item) 
    {
        if (is_array($item)) 
        {
            $ct += tree_count($item);
        }       
    }
    return $ct;
}

$testsdir = "/home/michael/dev/php_toml/tests/";

$p = new TomlParser();

    foreach ([
    'fruit.toml',
     'example.toml',
     'hard_example.toml',
     'hard_example_unicode.toml']
    as $name) {
        $data = file_get_contents( $testsdir . $name);
        try {
            $r = $p->parse($data);
            echo "Total keys = " . tree_count($r) . PHP_EOL;
        } catch (Exception $ex) {
            echo $ex->getMessage() . PHP_EOL;
        }
    }


$store = [];
foreach ([
    'fruit.toml',
     'example.toml',
     'hard_example.toml',
     'hard_example_unicode.toml']
    as $ix => $name) {
    $store[] = file_get_contents( $testsdir . $name);
}

$iterate = 50;

$start = microtime(true);
for($i = 0; $i < $iterate; $i++) {
    foreach($store as $data) {
        $r = $p->parse($data);
    }
}
$end = microtime(true);

$totaltime = $end - $start;
$avg = number_format(($totaltime * 1000) / $iterate,3);
echo 'Intepret ' . $totaltime . PHP_EOL;
echo "$avg ms" . PHP_EOL;


$p = new TomlReader();
$start = microtime(true);
for($i = 0; $i < $iterate; $i++) {
    foreach($store as $data) {
        $r = $p->read($data);
    }
}
$end = microtime(true);

$etime = $end - $start;

$avg = number_format(($etime * 1000) / $iterate,3);

echo 'C-Extension ' . $etime  . PHP_EOL;;
echo "$avg ms" . PHP_EOL;

echo 'Interpreter Slower ' . ($totaltime - $etime) / $etime . PHP_EOL;