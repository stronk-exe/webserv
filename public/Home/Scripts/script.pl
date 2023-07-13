#!/usr/bin/perl

use strict;
use warnings;

print "Content-type: text/html\n\n";
print "<html><body>\n";

print "<h1>Hello CGI Perl!</h1>\n";

print "<h2>Request Parameters:</h2>\n";
print "<ul>\n";

foreach my $param (param()) {
    my @values = param($param);
    print "<li><strong>$param:</strong> " . join(", ", @values) . "</li>\n";
}

print "</ul>\n";

print "</body></html>\n";
