#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# Create CGI object
my $cgi = CGI->new;

# Print Content-Type header
print $cgi->header;

# Check if a file was uploaded
if ($cgi->param('upload_file')) {
    my $filename = $cgi->param('upload_file');
    my $upload_filehandle = $cgi->upload('upload_file');
    open(my $file, '>', $filename) or die "Could not open file: $!";
    while (my $chunk = $upload_filehandle->getline()) {
        print $file $chunk;
    }
    close($file);
    print "File uploaded successfully.";
} else {
    print "No file uploaded.";
}