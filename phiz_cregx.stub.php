<?php


class DayTime {


    public function __construct(string $t24);
    public function set(int $hours, int $mins, float $seconds) : void {}
    public function split(int &$h24, int &$min, float &$sec) : void {}
    public function format(int $flags = 0) : string {}
    public function __toString() : string {}

}
/** @generate-function-entries */
class Cregx {

    public function __construct(string $exp, int $flags = 0, int $global = 0) {}

    public function __toString() : string {}

    public function match(string $subject, int $offset) : int {}

    public function captures() : ?array {}

    public function clear() : void {}

    public function setExpr(string $exp, int $flags = 0, int $global = 0) : void {}
}

/** @generate-function-entries */
function toml_parse(string $subject) : array {}

/** @generate-function-entries */
class Ctoml {

    public function matchInt(string $s) : int  {}
    public function matchDateTime(string $s) : DateTime  {}
    public function matchBool(string $s) : bool  {}
    public function matchFloatDot(string $s) : float {}
    public function matchFloatExp(string $s) : float  {}
}

function daytime_set(int $hours, int $mins, float $seconds) : float {}
function daytime_str(string $t24) : float {}
function daytime_split(float $t24, int &$h24, int &$min, float &$sec) : void {}
function daytime_format(float $t24, int $flags = 0) : string {}