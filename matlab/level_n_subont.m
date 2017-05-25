function [subont] = level_n_subont(ont, l)
    %LEVEL_N_SUBONT
    %
    %   [subont] = LEVEL_N_SUBONT(ont, l);
    %
    %       Creates a sub-ontology contain terms up to a specific level.
    %
    % Input
    % -----
    % [struct]
    % ont:  The ontology structure, see pfp_ontbuild.m.
    %
    % [double]
    % l:    A level.
    %
    % Output
    % ------
    % [struct]
    % subont:   The resulting sub-ontology.
    %
    % See also
    % --------
    % [>] pfp_ontbuild.m
    %
    % Dependency
    % ----------
    % [>] pfp_level.m
    % [>] pfp_ancestorterm.m
    % [>] pfp_subont.m

    % check inputs {{{
    if nargin ~= 2
        error('level_n_subont:InputCount', 'Expected 2 inputs.');
    end

    % ont
    validateattributes(ont, {'struct'}, {'nonempty'}, '', 'ont', 1);

    % l
    validateattributes(l, {'double'}, {'positive', 'integer'}, '', 'l', 2);
    % }}}

    level = pfp_level(ont, ont.term);
    subont = pfp_subont(ont, pfp_ancestorterm(ont, ont.term(level<=l)));
end

% -------------
% Yuxiang Jiang (yuxjiang@indiana.edu)
% Department of Computer Science
% Indiana University, Bloomington
% Last modified: Wed 24 May 2017 04:41:50 PM E
