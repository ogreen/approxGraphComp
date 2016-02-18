%% read teh data 
AS = csvread('overhead_2e-6.csv');


% draw the figure 
figure;

bar(100*AS,'stacked','EdgeColor',[.2 .2 .2],'LineWidth',1.02);

%%
hTitle  = title ('Fault Tolerant Algorithm Overhead at f=2^{-6}|E|');
hXLabel = xlabel('Graphs'                     );
hYLabel = ylabel('Relative Overhead (%)');    


set(gca,'XLim',[0, 16]);
% set(gca,'YLim',[-10, 400]);
set(gca,'YLim',[0, 100]);

set(gcf, 'Color', 'w'); %setting the back ground color

GraphNames={'astro-ph','audikw1','caidaRouterLevel','citationCiteseer','cnr-2000','coAuthorsDBLP','cond-mat-2005','coPapersDBLP','delaunay\_n18','5-scale20','G\_n\_pin\_pout','kron\_g500-simple-logn18','ldoor','preferentialAttachment','rgg\_n\_2\_18\_s0'};

set(gca, 'XTickLabel',GraphNames);

xticklabel_rotate([],45);


set(gca, ...
  'Box'         , 'off'     , ...
  'TickDir'     , 'out'     , ...
  'TickLength'  , [.02 .02] , ...
  'XMinorTick'  , 'off'      , ...
  'YMinorTick'  , 'off'      , ...
  'YGrid'       , 'on'      , ...
  'XColor'      , [.3 .3 .3], ...
  'YColor'      , [.3 .3 .3], ...
  'LineWidth'   , 1         );

legend('Auxiliary Data Structure Overhead', 'Fault Detection Overhead','Fault Correction Overhead') 
colormap hsv(4);