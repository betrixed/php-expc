<?php


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
    public function matchFloatDot(string $s) : double {}
    public function matchFloatExp(string $s) : double  {}
}