#!/usr/bin/perl

use strict;

# General SBB ###############################################################
my $paretoEpsilonTeam = 0.001;

my $teamPow = 3;

my $monolithic = 0;
my $splitLevel = 0; #split-level transfer

my $Msize = 8;
my $pmd = 0.7;
my $pma = 0.7;
my $pmm = 0.2;
my $pmn = 0.1;
my $omega = 30;
my $t = 3;
my $numLevels = 2;
my $Mgap = 4;

my $episodesPerGeneration = 10;

my $maxProgSize = 96;
my $pBidMutate = 1.0;
my $pBidSwap = 1.0;
my $pBidDelete = 0.5;
my $pBidAdd = 0.5;

my $statMod = 5;

my $validPhaseEpochs = 25;
my $testPhaseEpochs = 1000;

# SBB Diversity #############################################################
my $diversityMode = 7; # 0) normalized fitness only; 3) linear combination, 7) paretoScoreRanking
my $stateDiscretizationSteps = 3;
my $knnNovelty = 15;

# required for linear combination of fitness and novelty only
my $pNoveltyGeno = 0.2;
my $pNoveltyPheno = 0.4;

#required for classical fitness sharing only
my $sigmaShare = 0.05;

###################################################################################################
if(scalar(@ARGV) != 3)
{
    die "usage: genScript.pl prefix numRuns basePort";
}

my $prefix = $ARGV[0];
my $numRuns = $ARGV[1];
my $basePort = $ARGV[2];

print "prefix: $prefix\n";
print "numRuns: $numRuns\n";

my $run;
my $argFile;
my $nextSeed;

for($run = 0; $run < $numRuns; $run++)
{   
    $nextSeed = $basePort + ($run*1000);

    $argFile = "$prefix.$nextSeed.arg";
    
    open(ARG, ">$argFile") || die "cannot open $argFile";

    print ARG "paretoEpsilonTeam $paretoEpsilonTeam\n";
    print ARG "teamPow $teamPow\n";
    print ARG "monolithic $monolithic\n";
    print ARG "splitLevel $splitLevel\n";
    print ARG "\n";

    print ARG "Msize $Msize\n";
    print ARG "pmd $pmd\n";
    print ARG "pma $pma\n";
    print ARG "pmm $pmm\n";
    print ARG "pmn $pmn\n";
    print ARG "omega $omega\n";
    print ARG "t $t\n";
    print ARG "numLevels $numLevels\n";
    print ARG "Mgap $Mgap\n";
    print ARG "\n";

    print ARG "maxProgSize $maxProgSize\n";
    print ARG "pBidMutate $pBidMutate\n";
    print ARG "pBidSwap $pBidSwap\n";
    print ARG "pBidDelete $pBidDelete\n";
    print ARG "pBidAdd $pBidAdd\n";
    print ARG "\n";

    print ARG "statMod $statMod\n";
    print ARG "\n";

    print ARG "diversityMode $diversityMode\n";
    print ARG "stateDiscretizationSteps $stateDiscretizationSteps\n";
    print ARG "sigmaShare $sigmaShare\n";
    print ARG "pNoveltyGeno $pNoveltyGeno\n";
    print ARG "pNoveltyPheno $pNoveltyPheno\n";
    print ARG "knnNovelty $knnNovelty\n";

    print ARG "validPhaseEpochs $validPhaseEpochs\n";
    print ARG "testPhaseEpochs $testPhaseEpochs\n";
    print ARG "episodesPerGeneration $episodesPerGeneration\n";

    close(ARG);
}

