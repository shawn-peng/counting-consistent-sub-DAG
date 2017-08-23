% This script creates subontologies with terms up to a specific levels (depth)
% (from 1 to the full level/depth of the ontology)

% This script assumes:
% 1. the full ontology is stored as the variable 'ont'
% 2. if prefix is 'used', then 'uterms' is a list of used ontology terms.
%    notice that the list of 'uterms' has to be propagated.

% output folder:
% odir = '~/Projects/cdag/data/ontologies/levels';
odir = '~/temp';
ontstr = 'bpo';
prefix = 'used'; % 'all' or 'used'

L = pfp_level(ont); % full levels
ontologies = cell(1, L);
for l = 1 : L
    fprintf('level: %d\n', l);
    filename = fullfile(odir, ontstr, sprintf('%s_%s%d.txt', prefix, ontstr, l));
    fterm    = fullfile(odir, ontstr, sprintf('%s_%s%d_term.txt', prefix, ontstr, l));
    frel     = fullfile(odir, ontstr, sprintf('%s_%s%d_rel.txt', prefix, ontstr, l));
    subont = level_n_subont(ont, l);
    if strcmp(prefix, 'used')
        terms = {subont.term.id};
        used_terms = uterms(ismember(uterms, terms));
        subont = pfp_subont(subont, used_terms);
    end
    % keep the sub ontology
    ontologies{l} = subont;
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

clear odir ontstr prefix L l filename subont terms used_terms

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Sat 22 Jul 2017 09:40:50 AM E
