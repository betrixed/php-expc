# php-expc
Experimental classes for PHP as C-Extension. UTF8 String iterator. CArray for packed C types.

```php
$s = new Str8("Hello world, Καλημέρα κόσμε, コンニチハ");
foreach($s as $pos => $ucode) {
  echo $pos . "  " . dechex($ucode) . "  " . $s->byteslen() . " " . $s->bytes() . PHP_EOL;
}
```
