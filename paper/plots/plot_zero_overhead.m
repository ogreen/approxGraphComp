%% read teh data 
AS = csvread('zero-overhead.csv');

% draw the figure 
figure;

bar(AS(:,3),'FaceColor',[0 .5 .5],'EdgeColor',[.1 .1 .1],'LineWidth',1.2);

%%
hTitle  = title ('Overhead of Ft-SV in presence of no faults');
hXLabel = xlabel('Graphs'                     );
hYLabel = ylabel('Relative Overhead (in \%)');    


set(gca,'XLim',[0, 16]);
% set(gca,'YLim',[-10, 400]);
set(gca,'YLim',[0, 30]);

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
