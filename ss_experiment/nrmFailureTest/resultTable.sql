drop table if exists nrmfTable;
create table nrmfTable (
  id integer primary key autoincrement,
  graphName text not null,
  algmType text not null,          -- can be sync or Async 
  targetFailRate float not null,   -- the desired fail rate for TMR
  tmrFailRate float not null,      -- the measured fail rate for TMR when sampled
  numTrial integer not null, 
  normFaultRate integer not null,   -- the value of n corresponding to failure rate
  maxIter integer,
  numFiSvFailure integer, 
  numSSSvFailure integer,
  numSSHSvFailure integer,
  numTMRSvFailure integer,
  runDate smalldatetime,
  expName text    not null          -- name of experiment
);

