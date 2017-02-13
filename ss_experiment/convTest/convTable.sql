drop table if exists convTable;
create table convTable (
  id integer primary key autoincrement,
  graphName text not null,
  algmType text not null,          -- can be sync or Async 
  normFaultRate integer not null, 
  numTrial integer not null, 
  maxIter integer,
  ftAlgmType text not null,       -- can be FISV, SSSV, SSHSV or TMRSV
  ffNumIter integer,              -- number of iteration in fault free case
  ohIteration float not null,     --  (relative) overhead 
  runDate smalldatetime,
  experimentName text             -- may be used later to distinguish between different data collected
);