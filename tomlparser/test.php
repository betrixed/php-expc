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

$p = new TomlParser();
$output = new MapOutput();
foreach ([
'fruit.toml',
 'example.toml',
 'hard_example.toml',
 'hard_example_unicode.toml']
as $name) {
    $data = file_get_contents(__DIR__ . "/" . $name);

    $r = $p->parse($data);
    $output->aout($r);
}
