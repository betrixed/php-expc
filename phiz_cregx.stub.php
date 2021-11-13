<?php


/** @generate-function-entries */
class Cregx {

    public function __construct(string $exp, int $flags = 0, int $global = 0) {}

    public function __toString() : string {}

    public function match(string $subject, int $offset) : int {}

    public function captures() : ?array {}

    public function clear() : void {}

    public function regexpr(string $exp, int $flags = 0, int $global = 0) : void {}
}


function toml_parse(string $subject) : array {}