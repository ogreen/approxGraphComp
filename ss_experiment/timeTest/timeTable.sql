drop table if exists tTable;
create table tTable (
  id integer primary key autoincrement,
  graphName text not null,
  algmType text not null,          -- can be sync or Async 
  maxIter integer,
  ftAlgmType text,                -- can be FISV, SSSV, SSHSV or TMRSV
  ffNumIter integer,              -- number of iteration in fault free case
  lpTime float not null,          -- time for label propagation
  ssTime float not null,          -- time for self-stabilization 
  runDate smalldatetime,
  experimentName text             -- may be used later to distinguish between different data collected
);