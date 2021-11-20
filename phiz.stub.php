<?php

/** @generate-function-entries */


function route_extract_params(string $s): array {}


function str_camel(string $s, string $seperate = "_"): string {}


function str_uncamel(string $s, string $seperate = "_"): string {}


function str_utf8c(string $s, int $offset, int& $code) : string {};

function daytime_set(int $hours, int $mins, float $seconds) : float {}
function daytime_str(string $t24) : float {}
function daytime_split(float $t24, int &$h24, int &$min, float &$sec) : void {}
function daytime_format(float $t24, int $flags = 0) : string {}

