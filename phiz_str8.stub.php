<?php

/** @generate-class-entries */
function str_camel(string $s, string $seperate = "_"): string {}

/** @generate-class-entries */
function str_uncamel(string $s, string $seperate = "_"): string {}

/** @generate-class-entries */
class Str8 implements IteratorAggregate {
	public function __construct(string $s) {}

	/** @return string */
	public function __toString() {}


	public function getIterator(): Iterator {}
}
