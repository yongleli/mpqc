#
eval 'exec perl $0 $*'
    if 0;

##########################################################################

package QCParse;

sub testparse {
    my $parse = new QCParse;

    my $string = "x:
 xval
test_basis: STO-3G 6-311G**
charge: 1
method: scf
basis: sto-3g
state: 3b1
molecule:
  H 0 0.0000001 1.00000001
  H 0 0 -1
gradient: yes
optimize: no
frequencies: yes
properties: NPA
y:
yval
z: zval1 zval2
zval3
h:
0 a
1
 2  c";

    print "string:\n--------------\n$string\n--------------\n";

    $parse->parse_string($string);
    $parse->doprint();

    my @t = $parse->value_as_array('h');
    print "-----------------\n";
    for ($i = 0; $i <= $#t; $i++) {
        print "$i: $t[$i]\n";
    }
    print "-----------------\n";

    @t = $parse->value_as_lines('h');
    print "-----------------\n";
    for ($i = 0; $i <= $#t; $i++) {
        print "$i: $t[$i]\n";
    }
    print "-----------------\n";

    my $qcinp = new QCInput($parse);
    my $test_basis = $parse->value("test_basis");
    my @test_basis_a = $parse->value_as_array("test_basis");
    my $state = $qcinp->state();
    my $mult = $qcinp->mult();
    my $method = $qcinp->method();
    my $charge = $qcinp->charge();
    my $basis = $qcinp->basis();
    my $gradient = $qcinp->gradient();
    my $frequencies = $qcinp->frequencies();
    my $optimize = $qcinp->optimize();
    my $natom = $qcinp->n_atom();
    foreach $i (@test_basis_a) {
        print "test_basis_a: $i\n";
    }
    print "test_basis = $test_basis\n";
    print "state = $state\n";
    print "mult = $mult\n";
    print "method = $method\n";
    print "basis = $basis\n";
    print "optimize = $optimize\n";
    print "gradient = $gradient\n";
    print "frequencies = $frequencies\n";
    print "natom = $natom\n";
    for ($i = 0; $i < $natom; $i++) {
        printf "%s %14.8f %14.8f %14.8f\n", $qcinp->element($i),
                                $qcinp->position($i,0),
                                $qcinp->position($i,1),
                                $qcinp->position($i,2);
    }
    printf "qcinp errors: %s\n", $qcinp->error();

    my $inpwr = new MPQCInputWriter($qcinp);
    printf "MPQC input:\n%s", $inpwr->input_string();
}

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize();
    return $self;
}

sub initialize {
    my $self = shift;
    $self->{'keyval'} = {};
}

sub parse_file {
    my $self = shift;
    my $file = shift;
    open(INPUT, "<$file");
    my $string = "";
    while (<INPUT>) {
        $string = "$string$_";
    }
    close(INPUT);
    #print "Got file:\n$string\n";
    $self->parse_string($string);
}

sub parse_string {
    my $self = shift;
    my $string = shift;
    my $value = "";
    my $keyword = "";
    $string = "$string\n";
    while ($string) {
        $string =~ s/^[^\n]*\n//;
        $_ = $&;
        s/#.*//;
        if (/^\s*(\w+)\s*:\s*(.*)\s*$/) {
            $self->add($keyword, $value);
            $keyword = $1;
            $value = $2;
        }
        elsif (/^\s*$/) {
            $self->add($keyword, $value);
            $keyword = "";
            $value = "";
        }
        else {
            $value = "$value\n$_";
        }
    }
    $self->add($keyword, $value);
}

sub add {
    my $self = shift;
    my $keyword = shift;
    my $value = shift;
    if ($keyword ne "") {
        $self->{'keyval'}{$keyword} = $value;
    }
}

# returns the value of the keyword
sub value {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    return $value;
}

# sets the value of the keyword
sub set_value {
    my $self = shift;
    my $keyword = shift;
    my $value = shift;
    my $keyval = $self->{'keyval'};
    $keyval->{$keyword} = $value;
    return $value;
}

# returns the value of the keyword
sub boolean_value {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    $_ = $keyval->{$keyword};
    return "1" if (/^\s*(y|yes|1|true|t)\s*$/i);
    return "0" if (/^\s*(n|no|0|false|f|)\s*$/i);
    "";
}

# returns an array of whitespace delimited tokens
sub value_as_array {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    my @array = ();
    $i = 0;
    $value =~ s/^\s+$//;
    while ($value ne '') {
        $value =~ s/^\s*(\S+)\s*//s;
        $array[$i] = $1;
        $i++;
    }
    return @array;
}

# returns an array of lines
sub value_as_lines {
    my $self = shift;
    my $keyword = shift;
    my $keyval = $self->{'keyval'};
    my $value = $keyval->{$keyword};
    my @array = ();
    $i = 0;
    while ($value) {
        $value =~ s/^\s*(.*)\s*\n//;
        $array[$i] = $1;
        $i++;
    }
    return @array;
}

sub display {
    my $self = shift;
    my @keys = @_ ? @_ : sort keys %$self;
    foreach $key (@keys) {
        print "\t$key => $self->{$key}\n";
    }
}

sub doprint {
    my $self = shift;
    print "QCParse:\n";
    my $keyval = $self->{'keyval'};
    foreach $i (keys %$keyval) {
        my $val = $keyval->{$i};
        $val =~ s/\n/\\n/g;
        print "keyword = $i, value = $val\n";
    }
}

##########################################################################

package QCInput;
$debug = 0;

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize {
    my $self = shift;
    my $parser = shift;
    if ($parser eq "") {
        $parser = new QCParse;
    }
    $self->{"parser"} = $parser;
    $self->{"error"} = "";

    # parse the molecule information
    $self->{"position"} = [];
    $self->{"element"} = [];
    my @molecule = $parser->value_as_array("molecule");
    my $i = 0;
    while ($#molecule >= 0) {
        my $sym = shift @molecule;
        my $x = shift @molecule;
        my $y = shift @molecule;
        my $z = shift @molecule;
        $self->{"element"}[$i] = $sym;
        $self->{"position"}[$i] = [ $x, $y, $z ];
        $i++;
    }
    $self->{"natom"} = $i;

    print "QCInput: initialize: natom = $i\n" if ($debug);
}

sub error {
    my $self = shift;
    my $msg = shift;
    $self->{"error"} = "$self->{'error'}$msg";
}

sub charge {
    my $self = shift;
    $_ = $self->{"parser"}->value("charge");
    s/^\s+//;
    s/\s+$//;
    if (! /\d+/) {
        $self->error("Bad charge: $_ (using 0)\n");
        $_ = "0";
    }
    $_;
}

sub method {
    my $self = shift;
    $_ = $self->{"parser"}->value("method");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $self->error("No method given (using default).\n");
        $_ = "SCF";
    }
    tr/a-z/A-Z/;
    $_;
}

sub symmetry {
    my $self = shift;
    $_ = $self->{"parser"}->value("symmetry");
    s/^\s*//;
    s/\s*$//;
    uc $_;
}

sub state {
    my $self = shift;
    $_ = $self->{"parser"}->value("state");
    s/^\s*//;
    s/\s*$//;
    uc $_;
}

sub mult {
    my $self = shift;
    $_ = $self->state();
    s/(\d)\D.*$/\1/;
    if (/^\s*$/) {
        $_ = 1;
    }
    $_;
}

sub basis {
    my $self = shift;
    $_ = $self->{"parser"}->value("basis");
    s/^\s+//;
    s/\s+$//;
    if ($_ eq "") {
        $self->error("No basis given (using default).\n");
        $_ = "STO-3G";
    }
    tr/a-z/A-Z/;
    $_;
}

sub gradient {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("gradient");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("gradient");
        $self->error("Bad value for gradient: $val");
    }
    $bval;
}

sub optimize {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("optimize");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("optimize");
        $self->error("Bad value for optimize: $val");
    }
    $bval;
}

sub frequencies {
    my $self = shift;
    my $bval = $self->{"parser"}->boolean_value("frequencies");
    if ($bval eq "") {
        my $val = $self->{"parser"}->value("frequencies");
        $self->error("Bad value for frequencies: $val");
    }
    $bval;
}

sub n_atom {
    my $self = shift;
    printf "QCInput: returning natom = %d\n", $self->{"natom"} if ($debug);
    $self->{"natom"};
}

sub element {
    my $self = shift;
    my $i = shift;
    $self->{"element"}[$i];
}

sub position {
    my $self = shift;
    my $i = shift;
    my $xyz = shift;
    $self->{"position"}[$i][$xyz];
}

##########################################################################

package InputWriter;

# Input Writer is abstract
sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize() {
    my $self = shift;
    my $qcinput = shift;
    $self->{"qcinput"} = $qcinput;
}

# this should be overridden
sub input_string() {
    "";
}

sub write_input() {
    my $self = shift;
    my $file = shift;
    my $input = $self->input_string();
    open(OUTPUT,">$file");
    printf OUTPUT "%s", $input;
    close(OUTPUT);
}

##########################################################################

package MPQCInputWriter;
@ISA = qw( InputWriter );
%methodmap = ("MP2" => "MBPT2",
              "OPT2[2]" => "MBPT2",
              "ZAPT2" => "MBPT2",
              "ROSCF" => "SCF",
              "SCF" => "SCF" );
$debug = 0;

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;

    $self->initialize(@_);
    return $self;
}

sub initialize() {
    my $self = shift;
    my $qcinput = shift;
    $self->{"qcinput"} = $qcinput;
}

sub input_string() {
    my $self = shift;
    my $qcinput = $self->{"qcinput"};
    my $qcparse = $qcinput->{"parser"};

    printf "molecule = %s\n", $qcparse->value("molecule") if ($debug);

    my $symmetry = $qcinput->symmetry();
    my $mol = "% molecule specification";
    $mol = "$mol\nmolecule<Molecule>: (";
    $mol = "$mol\n  symmetry = $symmetry";
    $mol = "$mol\n  angstroms = yes";
    $mol = "$mol\n  { atoms geometry } = {";
    printf "MPQCInputWriter: natom = %d\n", $qcinput->n_atom() if ($debug);
    my $i;
    for ($i = 0; $i < $qcinput->n_atom(); $i++) {
        $mol = sprintf "%s\n    %s     [ %14.8f %14.8f %14.8f ]",
                       $mol, $qcinput->element($i),
                       $qcinput->position($i,0),
                       $qcinput->position($i,1),
                       $qcinput->position($i,2);
    }
    $mol = "$mol\n  }";
    $mol = "$mol\n)\n";

    my $basis = "% basis set specification";
    $basis = "$basis\nbasis<GaussianBasisSet>: (";
    $basis = sprintf "%s\n  name = \"%s\"", $basis, $qcinput->basis();
    $basis = "$basis\n  molecule = \$:molecule";
    $basis = "$basis\n)\n";

    my $method = $methodmap{uc($qcinput->method())};
    $method = "SCF" if ($method eq "");
    if ($method eq "SCF") {
        if ($qcinput->mult() == 1) {
            $method = "CLSCF";
        }
        else {
            $method = "HSOSSCF";
        }
    }
    my $mole = "  do_energy = yes";
    if ($qcinput->gradient()) {
        $mole = "$mole\n  do_gradient = yes";
    }
    else {
        $mole = "$mole\n  do_gradient = no";
    }
    $mole = "$mole\n  % method for computing the molecule's energy";
    $mole = "$mole\n  mole<$method>: (";
    $mole = "$mole\n    molecule = \$:molecule";
    $mole = "$mole\n    basis = \$:basis";
    if ($method eq "MBPT2") {
        my $refmethod = "";
        if ($qcinput->mult() == 1) {
            $refmethod = "CLSCF";
        }
        else {
            $refmethod = "HSOSSCF";
        }
        $mole = "$mole\n    reference<$refmethod>: (";
        $mole = "$mole\n      molecule = \$:molecule";
        $mole = "$mole\n      basis = \$:basis";
        $mole = "$mole\n    )";
    }
    $mole = "$mole\n  )\n";

    my $opt;
    if ($qcinput->optimize()) {
        $opt = "  optimize = yes";
    }
    else {
        $opt = "  optimize = no";
    }
    $opt = "$opt\n  % optimizer object for the molecular geometry";
    $opt = "$opt\n  opt<QNewtonOpt>: (";
    $opt = "$opt\n    function = \$..:mole";
    $opt = "$opt\n  )\n";

    my $freq = "";
    if ($qcinput->frequencies()) {
        $freq = "% vibrational frequency input";
        $freq = "$freq\n  freq<MolecularFrequencies>: (";
        $freq = "$freq\n    molecule = \$:molecule";
        $freq = "$freq\n  )\n";
    }

    my $mpqcstart = "mpqc: (\n";
    my $mpqcstop = ")\n";
    "$mol$basis$mpqcstart$mole$opt$freq$mpqcstop";
}

##########################################################################

package G94InputWriter;
@ISA = qw( InputWriter );

sub new {
    my $this = shift;
    my $class = ref($this) || $this;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub input_string() {
    my $self = shift;
}
