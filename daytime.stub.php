<?php

/** @generate-function-entries */
class DayTime {


    public function __construct(string $t24);
    public function set(int $hours, int $mins, float $seconds) : void {}
    public function split(int &$h24, int &$min, float &$sec) : void {}
    public function format(int $flags = 0) : string {}
    public function __toString() : string {}

}
