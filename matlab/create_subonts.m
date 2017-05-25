% this script creates subontologies with terms up to a specific levels (depth)
% (from 1 to the full level/depth of the ontology)

% assume the ontology is stored as the variable 'ont'

% output folder:
odir = '~/Projects/cdag/data/ontologies/levels';
ontstr = 'hpo';
prefix = 'used'; % 'all' or 'used'

L = pfp_level(ont); % full levels
for l = 1 : L
    fprintf('level: %d\n', l);
    filename = fullfile(odir, ontstr, sprintf('%s_%s%d.txt', prefix, ontstr, l));
    fterm    = fullfile(odir, ontstr, sprintf('%s_%s%d_term.txt', prefix, ontstr, l));
    frel     = fullfile(odir, ontstr, sprintf('%s_%s%d_rel.txt', prefix, ontstr, l));
    subont = level_n_subont(ont, l);
    pfp_saveont(filename, subont);

    % post processing
    system(sprintf('cut -f1,3 %s > /tmp/mytempfile; mv /tmp/mytempfile %s', frel, frel));
    if l == L && strcmp(prefix, 'all')
        new_fterm = fullfile(odir, ontstr, sprintf('%s.terms', ontstr));
        system(sprintf('mv %s %s', fterm, new_fterm));
    else
        system(sprintf('rm -f %s', fterm));
    end
end

clear odir ontstr prefix L l filename subont

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Thu 25 May 2017 02:48:05 PM E
