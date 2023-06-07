#!/usr/bin/php-cgi
<?php

// Set the content type to HTML and output the header
header("Content-Type: text/html");

// Output PHP information
echo "<h1>PHP Information</h1>";
echo "<hr>";
echo phpinfo();
