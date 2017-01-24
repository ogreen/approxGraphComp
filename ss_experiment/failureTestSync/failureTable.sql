drop table if exists fTable;
create table fTable (
  id integer primary key autoincrement,
  graphName text not null,
  algmType text not null,          -- can be sync or Async 
  normFaultRate integer not null, 
  numTrial integer not null, 
  maxIter integer,
  numFiSvFailure integer, 
  numSSSvFailure integer,
  numSSHSvFailure integer,
  numTMRSvFailure integer,
  runDate smalldatetime
);