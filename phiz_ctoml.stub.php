<?php

/** @generate-function-entries */
class Ctoml {

    public function matchInt(string $s) : int  {}
    public function matchDateTime(string $s) : DateTime  {}
    public function matchBool(string $s) : bool  {}
    public function matchFloatDot(string $s) : float {}
    public function matchFloatExp(string $s) : float  {}
}


/** @generate-function-entries */
function toml_parse(string $subject) : array {}
