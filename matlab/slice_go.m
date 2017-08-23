function [] = slice_go(gont, odir)
    %SLICE_GO
    %
    %   [] = SLICE_GO(gont, odir);
    %
    %       Slices Gene ontology into levels.
    %
    % Input
    % -----
    % [cell]
    % gont: 1-by-3 cell array of the three domains of gene ontology structure.
    %       We assume the order is: bpo, cco, mfo (alphabetically)
    %       See pfp_ontbuild.m
    %
    % [char]
    % odir: The output folder name.
    %
    % Output
    % ------
    % None.
    %
    % Dependency
    % ----------
    % [>] pfp_level.m
    % [>] level_n_subont.m
    %
    % See Also
    % --------
    % [>] pfp_ontbuild.m

    % check inputs {{{
    if nargin ~= 2
        error('slice_go:InputCount', 'Expected 2 inputs.');
    end

    % gont
    validateattributes(gont, {'cell'}, {'numel', 3}, '', 'gont', 1);

    % odir
    validateattributes(odir, {'char'}, {'nonempty'}, '', 'odir', 2);
    if ~exist(odir, 'dir')
        mkdir(odir);
    end
    % }}}

    % slice and save gene ontologies {{{
    % assuming gont = {bpo, cco, mfo}
    go_domain = {'bpo', 'cco', 'mfo'};
    for i = 1 : 3
        ont = gont{i};
        fprintf('processing %s ...\n', go_domain{i});
        L = pfp_level(ont);
        for l = 1 : L
            fprintf('level: %d\n', l);
            filename = fullfile(odir, sprintf('%s%d.txt', go_domain{i}, l));
            fterm    = fullfile(odir, sprintf('%s%d_term.txt', go_domain{i}, l));
            frel     = fullfile(odir, sprintf('%s%d_rel.txt', go_domain{i}, l));
            subont = level_n_subont(ont, l);
            pfp_saveont(filename, subont);

            % post processing
            system(sprintf('cut -f1,3 %s > /tmp/mytempfile; mv /tmp/mytempfile %s', frel, frel));
            % if l == L
            %     new_fterm = fullfile(odir, ontstr, sprintf('%s.terms', ontstr));
            %     system(sprintf('mv %s %s', fterm, new_fterm));
            % else
            %     system(sprintf('rm -f %s', fterm));
            % end
        end

    end
    % }}}
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Wed 21 Jun 2017 02:10:05 PM E
